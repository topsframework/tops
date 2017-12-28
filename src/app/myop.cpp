#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <dirent.h>

#include "model/DiscreteIIDModel.hpp"
#include "model/VariableLengthMarkovChain.hpp"
#include "model/InhomogeneousMarkovChain.hpp"
#include "model/PeriodicInhomogeneousMarkovChain.hpp"
#include "model/MultipleSequentialModel.hpp"
#include "model/MaximalDependenceDecomposition.hpp"

//Novos includes
#include "model/Util.hpp"
#include "model/Matrix.hpp"
#include "model/Sequence.hpp"
#include "model/Probability.hpp"
#include "model/SignalDuration.hpp"
#include "model/ExplicitDuration.hpp"
#include "model/GeometricDuration.hpp"
#include "exception/NotYetImplemented.hpp"
#include "helper/Sequence.hpp"
#include "helper/SExprTranslator.hpp"
#include "model/GeneralizedHiddenMarkovModel.hpp"
#include "model/VariableLengthMarkovChain.hpp"
#include "helper/VariableLengthMarkovChain.hpp"
#include "helper/DiscreteIIDModel.hpp"
#include "model/Standard.hpp"
#include "model/PhasedRunLengthDistribution.hpp"
#include <unordered_map>

using tops::model::Sequence;
using tops::model::ProbabilisticModelPtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::InhomogeneousMarkovChain;
using tops::model::InhomogeneousMarkovChainPtr;
using tops::model::PeriodicInhomogeneousMarkovChain;
using tops::model::PeriodicInhomogeneousMarkovChainPtr;
using tops::model::MultipleSequentialModel;
using tops::model::MultipleSequentialModelPtr;
using tops::model::Probability;
using tops::model::ContextTree;
using tops::model::MaximalDependenceDecomposition;
using tops::model::Consensus;

//Novos namespaces
using tops::model::GeneralizedHiddenMarkovModel;
using tops::model::GeneralizedHiddenMarkovModelPtr;
using tops::helper::createVLMCMC;
using tops::model::SignalDuration;
using tops::helper::createFairCoinIIDModel;
using tops::model::ExplicitDuration;
using tops::helper::createMachlerVLMC;
using tops::model::GeometricDuration;
using tops::model::Matrix;
using tops::model::Standard;
using tops::model::EvaluatorPtr;
using tops::model::Labeler;
using tops::helper::generateRandomIIDModel;
using tops::model::PhasedRunLengthDistribution;
using namespace std;

using Symbol = unsigned int;
using GHMM = GeneralizedHiddenMarkovModel;
using Id = typename GHMM::State::Id;
unsigned int GHMM_STATE_SIZE;

class Alphabet {
public:
    void addSymbol(std::string symbol) {
        _hash[symbol] = _vector.size();
        _vector.push_back(symbol);
    }

    virtual Symbol getID(std::string symbol) {
        return _hash[symbol];
    }

private:
    std::map<std::string, Symbol> _hash;
    std::vector<std::string> _vector;
};

using AlphabetPtr = std::shared_ptr<Alphabet>;

class IntegerAlphabet : public Alphabet {
    virtual Symbol getID(std::string symbol) {
        return std::stoi(symbol);
    }
};

using IntegerAlphabetPtr = std::shared_ptr<IntegerAlphabet>;

std::vector<std::vector<Symbol>> readSequence(std::string filename, std::vector<AlphabetPtr> alphabets) {
    std::vector<std::vector<Symbol>> sequence;
    std::ifstream file(filename);
    std::string line;
    bool first = true;
    while (std::getline(file, line)) {
        line += "\n";
        if (first) {
            int prev = -1;
            int i = 0;
            std::size_t found = line.find_first_of(" \t\n");
            while (found != std::string::npos) {
                if (found - prev - 1 > 0) {
                    sequence.push_back(std::vector<Symbol>{});
                    i++;
                }
                prev = found;
                found = line.find_first_of(" \t\n", found + 1);
            }
            first = false;
        } else {
            int prev = -1;
            int i = 0;
            std::size_t found = line.find_first_of(" \t\n");
            while (found != std::string::npos) {
                if (found - prev - 1 > 0) {
                    std::string sym = line.substr(prev + 1, found - prev - 1);
                    sequence[i].push_back(alphabets[i]->getID(sym));
                    i++;
                }
                prev = found;
                found = line.find_first_of(" \t\n", found + 1);
            }
        }
    }
    return sequence;
}

void generateDistributionChart(DiscreteIIDModelPtr iid, unsigned int start,
                               unsigned int end, std::string name) {
    std::stringstream y, x;
    y << "var " << name << "_duration_y = " << "[";
    x << "var " << name << "_duration_x = " << "[";
    for (auto i = start; i < end; i++) {
        y << iid->probabilityOf(i) << ",";
        x << i << ",";
    }
    y << "];" << std::endl;
    x << "];" << std::endl;

    std::ofstream f;
    f.open("output/" + name + "_duration.js");
    f << x.str() << y.str();
}

DiscreteIIDModelPtr trainDuration(std::string model_name, unsigned int max,
                                  unsigned int m, double slope) {
    auto trainer = DiscreteIIDModel::standardTrainer(
            DiscreteIIDModel::smoothed_histogram_stanke_algorithm{},
            std::vector<unsigned int>{1}, max, m, slope
    );

    trainer->add_training_sequence(readSequence(
            "dataset/" + model_name + "_duration/sequence.0.tsv",
            { std::make_shared<IntegerAlphabet>() }
                                   )[0]
    );

    auto model = trainer->train();

    generateDistributionChart(model, 0, max, model_name);

    return model;
}

ProbabilisticModelPtr trainNonCodingModel(AlphabetPtr nucleotides) {
    std::vector<Sequence> dataset;
    std::vector<double> weights;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("DATASET/noncoding")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence("dataset/noncoding/" + filename, {nucleotides})[0]);
                weights.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer = VariableLengthMarkovChain::standardTrainer(
            VariableLengthMarkovChain::interpolation_algorithm{},
            weights, 4, 3, 1, nullptr);

    for (auto s : dataset)
        trainer->add_training_sequence(s);

    return trainer->train();
}

ProbabilisticModelPtr trainRCDSModel(AlphabetPtr nucleotides) {
    std::vector<Sequence> dataset;
    std::vector<double> weights;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("dataset/rcds")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence("dataset/rcds/" + filename, {nucleotides})[0]);
                weights.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer = PeriodicInhomogeneousMarkovChain::standardTrainer(
            PeriodicInhomogeneousMarkovChain::interpolation_algorithm{},
            4, 4, 3, 1.0, weights, nullptr);

    for (auto s : dataset)
        trainer->add_training_sequence(s);

    return trainer->train();
}

ProbabilisticModelPtr trainCDSModel(AlphabetPtr nucleotides) {
    std::vector<Sequence> dataset;
    std::vector<double> weights;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("dataset/cds")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence("dataset/cds/" + filename, {nucleotides})[0]);
                weights.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer = PeriodicInhomogeneousMarkovChain::standardTrainer(
            PeriodicInhomogeneousMarkovChain::interpolation_algorithm{},
            4, 4, 3, 1.0, weights, nullptr);

    for (auto s : dataset)
        trainer->add_training_sequence(s);

    return trainer->train();
}

ProbabilisticModelPtr trainRStopModel(AlphabetPtr nucleotides) {
    std::vector<Sequence> dataset;
    std::vector<double> weights;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("dataset/rstop")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence("dataset/rstop/" + filename, {nucleotides})[0]);
                weights.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 2, 3, 0, 0, 0.0, Sequence{}, 0, weights);

    for (auto s : dataset)
        trainer->add_training_sequence(s);

    return trainer->train();
}

ProbabilisticModelPtr trainStopModel(AlphabetPtr nucleotides) {
    std::vector<Sequence> dataset;
    std::vector<double> weights;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("dataset/stop")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence("dataset/stop/" + filename, {nucleotides})[0]);
                weights.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 2, 3, 0, 0, 0.0, Sequence{}, 0, weights);

    for (auto s : dataset)
        trainer->add_training_sequence(s);

    return trainer->train();
}

ProbabilisticModelPtr trainRStartModel(AlphabetPtr nucleotides) {
    std::vector<Sequence> dataset;
    std::vector<double> weights;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("dataset/rstart_initiation_motif")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence("dataset/rstart_initiation_motif/" + filename, {nucleotides})[0]);
                weights.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 1, 20, 0, 0, 1.0, Sequence{}, 0, weights);

    for (auto s : dataset)
        trainer->add_training_sequence(s);

    auto initiation_motif = trainer->train();

    auto a_tree = ContextTree::make(4);
    auto a_root = a_tree->createContext();
    a_root->setDistribution(DiscreteIIDModel::make(
            std::vector<Probability>{{ 0.0, 1.0, 0.0, 0.0 }}));
    auto t_tree = ContextTree::make(4);
    auto t_root = t_tree->createContext();
    t_root->setDistribution(DiscreteIIDModel::make(
            std::vector<Probability>{{ 1.0, 0.0, 0.0, 0.0 }}));
    auto g_tree = ContextTree::make(4);
    auto g_root = g_tree->createContext();
    g_root->setDistribution(DiscreteIIDModel::make(
            std::vector<Probability>{{ 0.0, 0.0, 0.0, 1.0 }}));
    auto atg = InhomogeneousMarkovChain::make(
            std::vector<VariableLengthMarkovChainPtr>{
                    VariableLengthMarkovChain::make(a_tree),
                    VariableLengthMarkovChain::make(t_tree),
                    VariableLengthMarkovChain::make(g_tree)});

    std::vector<Sequence> dataset2;
    std::vector<double> weights2;
    if ((dir = opendir("dataset/rstart_initial_pattern")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset2.push_back(readSequence("dataset/rstart_initial_pattern/" + filename, {nucleotides})[0]);
                weights2.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer2 = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 3, 4, 0, 0, 1.0, Sequence{}, 0, weights2);

    for (auto s : dataset2)
        trainer2->add_training_sequence(s);

    auto initial_pattern = trainer2->train();

    return MultipleSequentialModel::make(std::vector<ProbabilisticModelPtr>{
            initial_pattern, atg, initiation_motif}, std::vector<int>{4, 3, 20});
}

ProbabilisticModelPtr trainStartModel(AlphabetPtr nucleotides) {
    std::vector<Sequence> dataset;
    std::vector<double> weights;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("dataset/start_initiation_motif")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence("dataset/start_initiation_motif/" + filename, {nucleotides})[0]);
                weights.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 1, 20, 0, 0, 1.0, Sequence{}, 0, weights);

    for (auto s : dataset)
        trainer->add_training_sequence(s);

    auto initiation_motif = trainer->train();

    auto a_tree = ContextTree::make(4);
    auto a_root = a_tree->createContext();
    a_root->setDistribution(DiscreteIIDModel::make(
            std::vector<Probability>{{ 1.0, 0.0, 0.0, 0.0 }}));
    auto t_tree = ContextTree::make(4);
    auto t_root = t_tree->createContext();
    t_root->setDistribution(DiscreteIIDModel::make(
            std::vector<Probability>{{ 0.0, 0.0, 0.0, 1.0 }}));
    auto g_tree = ContextTree::make(4);
    auto g_root = g_tree->createContext();
    g_root->setDistribution(DiscreteIIDModel::make(
            std::vector<Probability>{{ 0.0, 0.0, 1.0, 0.0 }}));
    auto atg = InhomogeneousMarkovChain::make(
            std::vector<VariableLengthMarkovChainPtr>{
                    VariableLengthMarkovChain::make(a_tree),
                    VariableLengthMarkovChain::make(t_tree),
                    VariableLengthMarkovChain::make(g_tree)});

    std::vector<Sequence> dataset2;
    std::vector<double> weights2;
    if ((dir = opendir("dataset/start_initial_pattern")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset2.push_back(readSequence("dataset/start_initial_pattern/" + filename, {nucleotides})[0]);
                weights2.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer2 = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 3, 4, 0, 0, 1.0, Sequence{}, 0, weights2);

    for (auto s : dataset2)
        trainer2->add_training_sequence(s);

    auto initial_pattern = trainer2->train();

    return MultipleSequentialModel::make(std::vector<ProbabilisticModelPtr>{
            initiation_motif, atg, initial_pattern}, std::vector<int>{20, 3, 4});
}

ProbabilisticModelPtr trainRDonorModel(AlphabetPtr nucleotides) {
    std::vector<Sequence> dataset;
    std::vector<double> weights;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("dataset/rdonor")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence("dataset/rdonor/" + filename, {nucleotides})[0]);
                weights.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 4, 9, 0, 0, 1.0, Sequence{3, 0}, 3, weights);

    for (auto s : dataset)
        trainer->add_training_sequence(s);

    auto donor = trainer->train();

    std::vector<Sequence> dataset2;
    std::vector<double> weights2;
    if ((dir = opendir("dataset/rdonor_initial_pattern")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset2.push_back(readSequence("dataset/rdonor_initial_pattern/" + filename, {nucleotides})[0]);
                weights2.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer2 = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 3, 4, 0, 0, 100.0, Sequence{}, 0, weights2);

    for (auto s : dataset2)
        trainer2->add_training_sequence(s);

    auto initial_pattern = trainer2->train();

    return MultipleSequentialModel::make(std::vector<ProbabilisticModelPtr>{
            donor, initial_pattern}, std::vector<int>{9, 4});
}

ProbabilisticModelPtr trainDonorModel(AlphabetPtr nucleotides) {
    std::vector<Sequence> dataset;
    DIR *dir;
    std::vector<double> weights;
    struct dirent *ent;
    if ((dir = opendir("dataset/donor")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence("dataset/donor/" + filename, {nucleotides})[0]);
                weights.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 4, 9, 0, 0, 1.0, Sequence{2, 3}, 3, weights);

    for (auto s : dataset)
        trainer->add_training_sequence(s);

    auto donor = trainer->train();

    std::vector<Sequence> dataset2;
    std::vector<double> weights2;
    if ((dir = opendir("dataset/donor_initial_pattern")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset2.push_back(readSequence("dataset/donor_initial_pattern/" + filename, {nucleotides})[0]);
                weights2.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer2 = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 3, 4, 0, 0, 100.0, Sequence{}, 0, weights2);

    for (auto s : dataset2)
        trainer2->add_training_sequence(s);

    auto initial_pattern = trainer2->train();

    return MultipleSequentialModel::make(std::vector<ProbabilisticModelPtr>{
            initial_pattern, donor}, std::vector<int>{4, 9});
}

ProbabilisticModelPtr trainRAcceptorModel(AlphabetPtr nucleotides) {
    std::vector<Sequence> dataset;
    std::vector<double> weights;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("dataset/racceptor_branch")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence("dataset/racceptor_branch/" + filename, {nucleotides})[0]);
                weights.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 3, 32, 0, 3, 1.0, Sequence{}, 0, weights);

    for (auto s : dataset)
        trainer->add_training_sequence(s);

    auto branch = trainer->train();

    std::vector<Sequence> dataset2;
    std::vector<double> weights2;
    if ((dir = opendir("dataset/racceptor")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset2.push_back(readSequence("dataset/racceptor/" + filename, {nucleotides})[0]);
                weights2.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer2 = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 5, 6, 0, 0, 1.0, Sequence{1, 3}, 1, weights2);

    for (auto s : dataset2)
        trainer2->add_training_sequence(s);

    auto acceptor = trainer2->train();

    std::vector<Sequence> dataset3;
    std::vector<double> weights3;
    if ((dir = opendir("dataset/racceptor_initial_pattern")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset3.push_back(readSequence("dataset/racceptor_initial_pattern/" + filename, {nucleotides})[0]);
                weights3.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer3 = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 3, 4, 0, 0, 100.0, Sequence{}, 0, weights3);

    for (auto s : dataset3)
        trainer3->add_training_sequence(s);

    auto initial_pattern = trainer3->train();

    return MultipleSequentialModel::make(std::vector<ProbabilisticModelPtr>{
            initial_pattern, acceptor, branch}, std::vector<int>{4, 6, 32});
}

//formado por 3 submodelos
ProbabilisticModelPtr trainAcceptorModel(AlphabetPtr nucleotides) {
    std::vector<Sequence> dataset;
    std::vector<double> weights;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("dataset/acceptor_branch")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence("dataset/acceptor_branch/" + filename, {nucleotides})[0]);
                weights.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 3, 32, 0, 3, 1.0, Sequence{}, 0, weights);

    for (auto s : dataset)
        trainer->add_training_sequence(s);

    auto branch = trainer->train();

    std::vector<Sequence> dataset2;
    std::vector<double> weights2;
    if ((dir = opendir("dataset/acceptor")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset2.push_back(readSequence("dataset/acceptor/" + filename, {nucleotides})[0]);
                weights2.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer2 = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 5, 6, 0, 0, 1.0, Sequence{0, 2}, 3, weights2);

    for (auto s : dataset2)
        trainer2->add_training_sequence(s);

    auto acceptor = trainer2->train();

    std::vector<Sequence> dataset3;
    std::vector<double> weights3;
    if ((dir = opendir("dataset/acceptor_initial_pattern")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset3.push_back(readSequence("dataset/acceptor_initial_pattern/" + filename, {nucleotides})[0]);
                weights3.push_back(1.0);
            }
        }
        closedir(dir);
    }

    auto trainer3 = InhomogeneousMarkovChain::standardTrainer(
            InhomogeneousMarkovChain::wam_algorithm{}, 4, 3, 4, 0, 0, 100.0, Sequence{}, 0, weights3);

    for (auto s : dataset3)
        trainer3->add_training_sequence(s);

    auto initial_pattern = trainer3->train();

    return MultipleSequentialModel::make(std::vector<ProbabilisticModelPtr>{
            branch, acceptor, initial_pattern}, std::vector<int>{32, 6, 4});
}

void generate_chart(ProbabilisticModelPtr model, std::string name,
                    Sequence sequence, unsigned int window_size = 1,
                    unsigned int start_phase = 0) {

    auto evaluator = model->standardEvaluator(sequence);

    std::stringstream y, x;
    y << "var " << name << "_y = " << "[";
    x << "var " << name << "_x = " << "[";
    for (auto i = 0u; i < sequence.size() - window_size + 1; i++) {
        y << evaluator->evaluateSequence(i, i + window_size, start_phase) << ",";
        x << i << ",";
    }
    y << "];" << std::endl;
    x << "];" << std::endl;

    std::ofstream f;
    f.open("output/" + name + ".js");
    f << x.str() << y.str();
}



std::vector<Probability> index_probabilities(std::vector<Id>& indices,
                                             std::vector<Probability>& probabilities) {

    std::vector<Probability> indexed_probabilities(GHMM_STATE_SIZE);
    for (std::size_t i = 0; i < indices.size(); i++)
        indexed_probabilities[indices[i]] = probabilities[i];
    return indexed_probabilities;
}

void print_probabilities(vector<Probability> probabilities ){
    for(unsigned int i = 0; i < probabilities.size(); i++){
        cout << probabilities[i] << " ";
    }
    cout << endl;
}

int main() {
    std::cerr << "|------------------------------------|" << std::endl;
    std::cerr << "|                MYOP                |" << std::endl;
    std::cerr << "|------------------------------------|" << std::endl;
    std::cerr << std::endl;
    //smart pointer "new"
    auto nucleotides = std::make_shared<Alphabet>();
    nucleotides->addSymbol("A");
    nucleotides->addSymbol("C");
    nucleotides->addSymbol("G");
    nucleotides->addSymbol("T");

    std::cerr << "- Training initial exon duration model... " << std::flush;

    auto exon_initial_duration = trainDuration("exon_initial", 15000, 10, 0.2);

    std::cerr << "done!" << std::endl;

    std::cerr << "- Training final exon duration model... " << std::flush;

    auto exon_final_duration = trainDuration("exon_final", 15000, 10, 0.2);

    std::cerr << "done!" << std::endl;

    std::cerr << "- Training internal exon duration model... " << std::flush;

    auto exon_internal_duration = trainDuration("exon_internal", 15000, 10, 0.2);

    std::cerr << "done!" << std::endl;

    std::cerr << "- Training single exon duration model... " << std::flush;

    auto exon_single_duration = trainDuration("exon_single", 15000, 10, 0.2);

    std::cerr << "done!" << std::endl;

    std::cerr << "- Training short intron duration model... " << std::flush;

    auto intron_short_duration = trainDuration("intron_short", 585, 2, 0.4);

    std::cerr << "done!" << std::endl;

    std::cerr << "- Training non-coding model... " << std::flush;

    auto non_coding_model = trainNonCodingModel(nucleotides);

    std::cerr << "done!" << std::endl;

    std::cerr << "- Training coding models... " << std::flush;

    auto rcds_model = trainRCDSModel(nucleotides);
    auto cds_model = trainCDSModel(nucleotides);

    std::cerr << "done!" << std::endl;

    std::cerr << "- Training stop codon models... " << std::flush;

    auto rstop_model = trainRStopModel(nucleotides);
    auto stop_model = trainStopModel(nucleotides);

    std::cerr << "done!" << std::endl;

    std::cerr << "- Training start codon models... " << std::flush;

    auto rstart_model = trainRStartModel(nucleotides);
    auto start_model = trainStartModel(nucleotides);

    std::cerr << "done!" << std::endl;

    std::cerr << "- Training donor models... " << std::flush;

    auto rdonor_model = trainRDonorModel(nucleotides);
    auto donor_model = trainDonorModel(nucleotides);

    std::cerr << "done!" << std::endl;

    std::cerr << "- Training acceptor models... " << std::flush;

    auto racceptor_model = trainRAcceptorModel(nucleotides);
    auto acceptor_model = trainAcceptorModel(nucleotides);

    std::cerr << "done!" << std::endl;

    /////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> state_names {
            "N","I0","I1","I2",
            "EI0","EI1","EI2","ES", "Is0", "If0",   "Is1", "If1",  "Is2", "If2",
            "ET0","ET1","ET2",  "rIs0", "rIf0",  "rIs1", "rIf1", "rIs2", "rIf2",
            "start", "stop", "acc0", "acc1", "acc2", "don0", "don1", "don2",
            "E00", "E01", "E02", "E10", "E11", "E12","E20", "E21", "E22",
            "rI0","rI1", "rI2",
            "rEI0","rEI1","rEI2","rES",
            "rET0","rET1","rET2",
            "rstart", "rstop", "racc0", "racc1", "racc2", "rdon0", "rdon1", "rdon2",
            "rE00", "rE01", "rE02", "rE10", "rE11", "rE12","rE20", "rE21", "rE22", "F"
    };

    GHMM_STATE_SIZE = state_names.size();


    std::unordered_map<std::string, Symbol> state_indices;

    for (unsigned int index = 0; index < state_names.size(); index++) {
       state_indices.emplace(state_names[index], index);
    }

    //State I0 definition
    auto I0_probabilities = vector<Probability>{{0.998908491611056, 0.00109150838894445}};
    auto I0_probabilities_indices = vector<Id> {state_indices["I0"],
                                                state_indices["acc0"]};
    auto I0_indexed_transitions_probabilities = index_probabilities(I0_probabilities_indices,
                                                                    I0_probabilities);

    print_probabilities(I0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr I0_transitions_probabilities
            = DiscreteIIDModel::make(I0_indexed_transitions_probabilities);

    GHMM::StatePtr I0
            = GHMM::State::make(
                    state_indices["I0"],
                    non_coding_model,
                    I0_transitions_probabilities,
                    GeometricDuration::make(state_indices["I0"],
                                            I0_transitions_probabilities));

    //State I1 definition
    auto I1_probabilities = vector<Probability>{{0.998908491611056, 0.00109150838894445}};
    auto I1_probabilities_indices = vector<unsigned int> {state_indices["I1"], state_indices["acc1"]};
    auto I1_indexed_transitions_probabilities = index_probabilities(I1_probabilities_indices, I1_probabilities);

    print_probabilities(I1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr I1_transitions_probabilities
            = DiscreteIIDModel::make(I1_indexed_transitions_probabilities);

    GHMM::StatePtr I1
            = GHMM::State::make(
                    state_indices["I1"],
                    non_coding_model,
                    I1_transitions_probabilities,
                    GeometricDuration::make(state_indices["I1"],
                                            I1_transitions_probabilities));

    //State I2 definition
    auto I2_probabilities = vector<Probability>{{0.998908491611056, 0.00109150838894445}};
    auto I2_probabilities_indices = vector<unsigned int> {state_indices["I2"], state_indices["acc2"]};
    auto I2_indexed_transitions_probabilities = index_probabilities(I2_probabilities_indices, I2_probabilities);

    print_probabilities(I2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr I2_transitions_probabilities
            = DiscreteIIDModel::make(I2_indexed_transitions_probabilities);

    GHMM::StatePtr I2
            = GHMM::State::make(
                    state_indices["I2"],
                    non_coding_model,
                    I2_transitions_probabilities,
                    GeometricDuration::make(state_indices["I2"],
                                            I2_transitions_probabilities));

    //State Is0 definition
    auto Is0_probabilities = vector<Probability>{{1}};
    auto Is0_probabilities_indices = vector<unsigned int> {state_indices["acc0"]};
    auto Is0_indexed_transitions_probabilities = index_probabilities(Is0_probabilities_indices, Is0_probabilities);

    print_probabilities(Is0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr Is0_transitions_probabilities
            = DiscreteIIDModel::make(Is0_indexed_transitions_probabilities);

    GHMM::StatePtr Is0
            = GHMM::State::make(
                    state_indices["Is0"],
                    non_coding_model,
                    Is0_transitions_probabilities,
                    ExplicitDuration::make(intron_short_duration));

    //State Is1 definition
    auto Is1_probabilities = vector<Probability>{{1}};
    auto Is1_probabilities_indices = vector<unsigned int> {state_indices["acc1"]};
    auto Is1_indexed_transitions_probabilities = index_probabilities(Is1_probabilities_indices, Is1_probabilities);

    print_probabilities(Is1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr Is1_transitions_probabilities
            = DiscreteIIDModel::make(Is1_indexed_transitions_probabilities);

    GHMM::StatePtr Is1
            = GHMM::State::make(
                    state_indices["Is1"],
                    non_coding_model,
                    Is1_transitions_probabilities,
                    ExplicitDuration::make(intron_short_duration));

    //State Is2 definition
    auto Is2_probabilities = vector<Probability>{{1}};
    auto Is2_probabilities_indices = vector<unsigned int> {state_indices["acc2"]};
    auto Is2_indexed_transitions_probabilities = index_probabilities(Is2_probabilities_indices, Is2_probabilities);

    print_probabilities(Is2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr Is2_transitions_probabilities
            = DiscreteIIDModel::make(Is2_indexed_transitions_probabilities);

    GHMM::StatePtr Is2
            = GHMM::State::make(
                    state_indices["Is2"],
                    non_coding_model,
                    Is2_transitions_probabilities,
                    ExplicitDuration::make(intron_short_duration));

    //State If0 definition
    auto If0_probabilities = vector<Probability>{{1}};
    auto If0_probabilities_indices = vector<unsigned int> {state_indices["I0"]};
    auto If0_indexed_transitions_probabilities = index_probabilities(If0_probabilities_indices, If0_probabilities);

    print_probabilities(If0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr If0_transitions_probabilities
            = DiscreteIIDModel::make(If0_indexed_transitions_probabilities);

    GHMM::StatePtr If0
            = GHMM::State::make(
                    state_indices["If0"],
                    non_coding_model,
                    If0_transitions_probabilities,
                    SignalDuration::make(584));

    //State If1 definition
    auto If1_probabilities = vector<Probability>{{1}};
    auto If1_probabilities_indices = vector<unsigned int> {state_indices["I1"]};
    auto If1_indexed_transitions_probabilities = index_probabilities(If1_probabilities_indices, If1_probabilities);

    print_probabilities(If1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr If1_transitions_probabilities
            = DiscreteIIDModel::make(If1_indexed_transitions_probabilities);

    GHMM::StatePtr If1
            = GHMM::State::make(
                    state_indices["If1"],
                    non_coding_model,
                    If1_transitions_probabilities,
                    SignalDuration::make(584));

    //State If2 definition
    auto If2_probabilities = vector<Probability>{{1}};
    auto If2_probabilities_indices = vector<unsigned int> {state_indices["I2"]};
    auto If2_indexed_transitions_probabilities = index_probabilities(If2_probabilities_indices, If2_probabilities);

    print_probabilities(If2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr If2_transitions_probabilities
            = DiscreteIIDModel::make(If2_indexed_transitions_probabilities);

    GHMM::StatePtr If2
            = GHMM::State::make(
                    state_indices["If2"],
                    non_coding_model,
                    If2_transitions_probabilities,
                    SignalDuration::make(584));

    //State start definition
    auto start_probabilities = vector<Probability>{{0.3, 0.3, 0.3, 0.1}};
    auto start_probabilities_indices = vector<unsigned int> {state_indices["EI0"], state_indices["EI1"],
                                                             state_indices["EI2"], state_indices["ES"]};
    auto start_indexed_transitions_probabilities = index_probabilities(start_probabilities_indices, start_probabilities);

    print_probabilities(start_indexed_transitions_probabilities);

    DiscreteIIDModelPtr start_transitions_probabilities
            = DiscreteIIDModel::make(start_indexed_transitions_probabilities);

    GHMM::StatePtr start
            = GHMM::State::make(
                    state_indices["start"],
                    start_model,
                    start_transitions_probabilities,
                    SignalDuration::make(27));

    //State EI0 definition
    auto EI0_probabilities = vector<Probability>{{1}};
    auto EI0_probabilities_indices = vector<unsigned int> {state_indices["don1"]};
    auto EI0_indexed_transitions_probabilities = index_probabilities(EI0_probabilities_indices, EI0_probabilities);

    print_probabilities(EI0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr EI0_transitions_probabilities
            = DiscreteIIDModel::make(EI0_indexed_transitions_probabilities);

    GHMM::StatePtr EI0
            = GHMM::State::make(
                    state_indices["EI0"],
                    cds_model,
                    EI0_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_initial_duration,
                                                                                                 11, 0, 0, 3)));

    //State EI1 definition
    auto EI1_probabilities = vector<Probability>{{1}};
    auto EI1_probabilities_indices = vector<unsigned int> {state_indices["don2"]};
    auto EI1_indexed_transitions_probabilities = index_probabilities(EI1_probabilities_indices, EI1_probabilities);

    print_probabilities(EI1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr EI1_transitions_probabilities
            = DiscreteIIDModel::make(EI1_indexed_transitions_probabilities);

    GHMM::StatePtr EI1
            = GHMM::State::make(
                    state_indices["EI1"],
                    cds_model,
                    EI1_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_initial_duration,
                                                                                                 11, 0, 1, 3)));
    //State EI2 definition
    auto EI2_probabilities = vector<Probability>{{1}};
    auto EI2_probabilities_indices = vector<unsigned int> {state_indices["don0"]};
    auto EI2_indexed_transitions_probabilities = index_probabilities(EI2_probabilities_indices, EI2_probabilities);

    print_probabilities(EI2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr EI2_transitions_probabilities
            = DiscreteIIDModel::make(EI2_indexed_transitions_probabilities);

    GHMM::StatePtr EI2
            = GHMM::State::make(
                    state_indices["EI2"],
                    cds_model,
                    EI2_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_initial_duration,
                                                                                                 11, 0, 2, 3)));
    //State ET0 definition
    auto ET0_probabilities = vector<Probability>{{1}};
    auto ET0_probabilities_indices = vector<unsigned int> {state_indices["stop"]};
    auto ET0_indexed_transitions_probabilities = index_probabilities(ET0_probabilities_indices, ET0_probabilities);

    print_probabilities(ET0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr ET0_transitions_probabilities
            = DiscreteIIDModel::make(ET0_indexed_transitions_probabilities);

    GHMM::StatePtr ET0
            = GHMM::State::make(
                    state_indices["ET0"],
                    cds_model,
                    ET0_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_final_duration,
                                                                                                 5, 0, 2, 3)));
    //State ET1 definition
    auto ET1_probabilities = vector<Probability>{{1}};
    auto ET1_probabilities_indices = vector<unsigned int> {state_indices["stop"]};
    auto ET1_indexed_transitions_probabilities = index_probabilities(ET1_probabilities_indices, ET1_probabilities);

    print_probabilities(ET1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr ET1_transitions_probabilities
            = DiscreteIIDModel::make(ET1_indexed_transitions_probabilities);

    GHMM::StatePtr ET1
            = GHMM::State::make(
                    state_indices["ET1"],
                    cds_model,
                    ET1_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_final_duration,
                                                                                                 5, 1, 2, 3)));
    //State ET2 definition
    auto ET2_probabilities = vector<Probability>{{1}};
    auto ET2_probabilities_indices = vector<unsigned int> {state_indices["stop"]};
    auto ET2_indexed_transitions_probabilities = index_probabilities(ET2_probabilities_indices, ET2_probabilities);

    print_probabilities(ET2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr ET2_transitions_probabilities
            = DiscreteIIDModel::make(ET2_indexed_transitions_probabilities);

    GHMM::StatePtr ET2
            = GHMM::State::make(
                    state_indices["ET2"],
                    cds_model,
                    ET2_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_final_duration,
                                                                                                 5, 2, 2, 3)));
    //State E00 definition
    auto E00_probabilities = vector<Probability>{{1}};
    auto E00_probabilities_indices = vector<unsigned int> {state_indices["don1"]};
    auto E00_indexed_transitions_probabilities = index_probabilities(E00_probabilities_indices, E00_probabilities);

    print_probabilities(E00_indexed_transitions_probabilities);

    DiscreteIIDModelPtr E00_transitions_probabilities
            = DiscreteIIDModel::make(E00_indexed_transitions_probabilities);

    GHMM::StatePtr E00
            = GHMM::State::make(
                    state_indices["E00"],
                    cds_model,
                    E00_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 0, 3)));
    //State E01 definition
    auto E01_probabilities = vector<Probability>{{1}};
    auto E01_probabilities_indices = vector<unsigned int> {state_indices["don2"]};
    auto E01_indexed_transitions_probabilities = index_probabilities(E01_probabilities_indices, E01_probabilities);

    print_probabilities(E01_indexed_transitions_probabilities);

    DiscreteIIDModelPtr E01_transitions_probabilities
            = DiscreteIIDModel::make(E01_indexed_transitions_probabilities);

    GHMM::StatePtr E01
            = GHMM::State::make(
                    state_indices["E01"],
                    cds_model,
                    E01_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 1, 3)));

    //State E02 definition
    auto E02_probabilities = vector<Probability>{{1}};
    auto E02_probabilities_indices = vector<unsigned int> {state_indices["don0"]};
    auto E02_indexed_transitions_probabilities = index_probabilities(E02_probabilities_indices, E02_probabilities);

    print_probabilities(E02_indexed_transitions_probabilities);

    DiscreteIIDModelPtr E02_transitions_probabilities
            = DiscreteIIDModel::make(E02_indexed_transitions_probabilities);

    GHMM::StatePtr E02
            = GHMM::State::make(
                    state_indices["E02"],
                    cds_model,
                    E02_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 2, 3)));
    //State E10 definition
    auto E10_probabilities = vector<Probability>{{1}};
    auto E10_probabilities_indices = vector<unsigned int> {state_indices["don1"]};
    auto E10_indexed_transitions_probabilities = index_probabilities(E10_probabilities_indices, E10_probabilities);

    print_probabilities(E10_indexed_transitions_probabilities);

    DiscreteIIDModelPtr E10_transitions_probabilities
            = DiscreteIIDModel::make(E10_indexed_transitions_probabilities);

    GHMM::StatePtr E10
            = GHMM::State::make(
                    state_indices["E10"],
                    cds_model,
                    E10_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 2, 3)));
    //State E11 definition
    auto E11_probabilities = vector<Probability>{{1}};
    auto E11_probabilities_indices = vector<unsigned int> {state_indices["don2"]};
    auto E11_indexed_transitions_probabilities = index_probabilities(E11_probabilities_indices, E11_probabilities);

    print_probabilities(E11_indexed_transitions_probabilities);

    DiscreteIIDModelPtr E11_transitions_probabilities
            = DiscreteIIDModel::make(E11_indexed_transitions_probabilities);

    GHMM::StatePtr E11
            = GHMM::State::make(
                    state_indices["E11"],
                    cds_model,
                    E11_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 0, 3)));
    //State E12 definition
    auto E12_probabilities = vector<Probability>{{1}};
    auto E12_probabilities_indices = vector<unsigned int> {state_indices["don0"]};
    auto E12_indexed_transitions_probabilities = index_probabilities(E12_probabilities_indices, E12_probabilities);

    print_probabilities(E12_indexed_transitions_probabilities);

    DiscreteIIDModelPtr E12_transitions_probabilities
            = DiscreteIIDModel::make(E12_indexed_transitions_probabilities);

    GHMM::StatePtr E12
            = GHMM::State::make(
                    state_indices["E12"],
                    cds_model,
                    E12_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 1, 3)));
    //State E20 definition
    auto E20_probabilities = vector<Probability>{{1}};
    auto E20_probabilities_indices = vector<unsigned int> {state_indices["don1"]};
    auto E20_indexed_transitions_probabilities = index_probabilities(E20_probabilities_indices, E20_probabilities);

    print_probabilities(E20_indexed_transitions_probabilities);

    DiscreteIIDModelPtr E20_transitions_probabilities
            = DiscreteIIDModel::make(E20_indexed_transitions_probabilities);

    GHMM::StatePtr E20
            = GHMM::State::make(
                    state_indices["E20"],
                    cds_model,
                    E20_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 1, 3)));
    //State E21 definition
    auto E21_probabilities = vector<Probability>{{1}};
    auto E21_probabilities_indices = vector<unsigned int> {state_indices["don2"]};
    auto E21_indexed_transitions_probabilities = index_probabilities(E21_probabilities_indices, E21_probabilities);

    print_probabilities(E21_indexed_transitions_probabilities);

    DiscreteIIDModelPtr E21_transitions_probabilities
            = DiscreteIIDModel::make(E21_indexed_transitions_probabilities);

    GHMM::StatePtr E21
            = GHMM::State::make(
                    state_indices["E21"],
                    cds_model,
                    E21_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 2, 3)));
    //State E22 definition
    auto E22_probabilities = vector<Probability>{{1}};
    auto E22_probabilities_indices = vector<unsigned int> {state_indices["don0"]};
    auto E22_indexed_transitions_probabilities = index_probabilities(E22_probabilities_indices, E22_probabilities);

    print_probabilities(E22_indexed_transitions_probabilities);

    DiscreteIIDModelPtr E22_transitions_probabilities
            = DiscreteIIDModel::make(E22_indexed_transitions_probabilities);

    GHMM::StatePtr E22
            = GHMM::State::make(
                    state_indices["E22"],
                    cds_model,
                    E22_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 0, 3)));

    //State ES definition
    auto ES_probabilities = vector<Probability>{{1}};
    auto ES_probabilities_indices = vector<unsigned int> {state_indices["stop"]};
    auto ES_indexed_transitions_probabilities = index_probabilities(ES_probabilities_indices, ES_probabilities);

    print_probabilities(ES_indexed_transitions_probabilities);

    DiscreteIIDModelPtr ES_transitions_probabilities
            = DiscreteIIDModel::make(ES_indexed_transitions_probabilities);

    GHMM::StatePtr ES
            = GHMM::State::make(
                    state_indices["ES"],
                    cds_model,
                    ES_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_single_duration,
                                                                                                 4, 0, 2, 3)));
    //State N definition
    auto N_probabilities = vector<Probability>{{9.95e-05, 0.999882352941176, 9.0735294117647e-06, 9.0735294117647e-06}};
    auto N_probabilities_indices = vector<Id> {state_indices["F"], state_indices["N"],
                                               state_indices["rstop"], state_indices["start"]};
    auto N_indexed_transitions_probabilities = index_probabilities(N_probabilities_indices, N_probabilities);

    print_probabilities(N_indexed_transitions_probabilities);

    DiscreteIIDModelPtr N_transitions_probabilities
            = DiscreteIIDModel::make(N_indexed_transitions_probabilities);

    GHMM::StatePtr N
            = GHMM::State::make(
                    state_indices["N"],
                    non_coding_model,
                    N_transitions_probabilities,
                    GeometricDuration::make(state_indices["N"],
                                            N_transitions_probabilities));

    //State stop definition
    auto stop_probabilities = vector<Probability>{{1}};
    auto stop_probabilities_indices = vector<unsigned int> {state_indices["N"]};
    auto stop_indexed_transitions_probabilities = index_probabilities(stop_probabilities_indices, stop_probabilities);

    print_probabilities(stop_indexed_transitions_probabilities);

    DiscreteIIDModelPtr stop_transitions_probabilities
            = DiscreteIIDModel::make(stop_indexed_transitions_probabilities);

    GHMM::StatePtr stop
            = GHMM::State::make(
                    state_indices["stop"],
                    stop_model,
                    stop_transitions_probabilities,
                    SignalDuration::make(3));

    //State don0 definition
    auto don0_probabilities = vector<Probability>{{0.144160536105428, 0.855839463894572}};
    auto don0_probabilities_indices = vector<unsigned int> {state_indices["If0"], state_indices["Is0"]};
    auto don0_indexed_transitions_probabilities = index_probabilities(don0_probabilities_indices, don0_probabilities);

    print_probabilities(don0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr don0_transitions_probabilities
            = DiscreteIIDModel::make(don0_indexed_transitions_probabilities);

    GHMM::StatePtr don0
            = GHMM::State::make(
                    state_indices["don0"],
                    donor_model,
                    don0_transitions_probabilities,
                    SignalDuration::make(13));

    //State don1 definition
    auto don1_probabilities = vector<Probability>{{0.144160536105428, 0.855839463894572}};
    auto don1_probabilities_indices = vector<unsigned int> {state_indices["If1"], state_indices["Is1"]};
    auto don1_indexed_transitions_probabilities = index_probabilities(don1_probabilities_indices, don1_probabilities);

    print_probabilities(don1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr don1_transitions_probabilities
            = DiscreteIIDModel::make(don1_indexed_transitions_probabilities);

    GHMM::StatePtr don1
            = GHMM::State::make(
                    state_indices["don1"],
                    donor_model,
                    don1_transitions_probabilities,
                    SignalDuration::make(13));

    //State don2 definition
    auto don2_probabilities = vector<Probability>{{0.144160536105428, 0.855839463894572}};
    auto don2_probabilities_indices = vector<unsigned int> {state_indices["If2"], state_indices["Is2"]};
    auto don2_indexed_transitions_probabilities = index_probabilities(don2_probabilities_indices, don2_probabilities);

    print_probabilities(don2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr don2_transitions_probabilities
            = DiscreteIIDModel::make(don2_indexed_transitions_probabilities);

    GHMM::StatePtr don2
            = GHMM::State::make(
                    state_indices["don2"],
                    donor_model,
                    don2_transitions_probabilities,
                    SignalDuration::make(13));

    //State acc0 definition
    auto acc0_probabilities = vector<Probability>{{0.3, 0.3, 0.3, 0.1}};
    auto acc0_probabilities_indices = vector<unsigned int> {state_indices["E00"], state_indices["E01"],
                                                            state_indices["E02"], state_indices["ET0"]};
    auto acc0_indexed_transitions_probabilities = index_probabilities(acc0_probabilities_indices, acc0_probabilities);

    print_probabilities(acc0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr acc0_transitions_probabilities
            = DiscreteIIDModel::make(acc0_indexed_transitions_probabilities);

    GHMM::StatePtr acc0
            = GHMM::State::make(
                    state_indices["acc0"],
                    acceptor_model,
                    acc0_transitions_probabilities,
                    SignalDuration::make(42));

    //State acc1 definition
    auto acc1_probabilities = vector<Probability>{{0.3, 0.3, 0.3, 0.1}};
    auto acc1_probabilities_indices = vector<unsigned int> {state_indices["E10"], state_indices["E11"],
                                                            state_indices["E12"], state_indices["ET1"]};
    auto acc1_indexed_transitions_probabilities = index_probabilities(acc1_probabilities_indices, acc1_probabilities);

    print_probabilities(acc1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr acc1_transitions_probabilities
            = DiscreteIIDModel::make(acc1_indexed_transitions_probabilities);

    GHMM::StatePtr acc1
            = GHMM::State::make(
                    state_indices["acc1"],
                    acceptor_model,
                    acc1_transitions_probabilities,
                    SignalDuration::make(42));

    //State acc2 definition
    auto acc2_probabilities = vector<Probability>{{0.3, 0.3, 0.3, 0.1}};
    auto acc2_probabilities_indices = vector<unsigned int> {state_indices["E20"], state_indices["E21"],
                                                            state_indices["E22"], state_indices["ET2"]};
    auto acc2_indexed_transitions_probabilities = index_probabilities(acc2_probabilities_indices, acc2_probabilities);

    print_probabilities(acc2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr acc2_transitions_probabilities
            = DiscreteIIDModel::make(acc2_indexed_transitions_probabilities);

    GHMM::StatePtr acc2
            = GHMM::State::make(
                    state_indices["acc2"],
                    acceptor_model,
                    acc2_transitions_probabilities,
                    SignalDuration::make(42));

    //State rET0 definition
    auto rET0_probabilities = vector<Probability>{{1}};
    auto rET0_probabilities_indices = vector<unsigned int> {state_indices["racc0"]};
    auto rET0_indexed_transitions_probabilities = index_probabilities(rET0_probabilities_indices, rET0_probabilities);

    print_probabilities(rET0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rET0_transitions_probabilities
            = DiscreteIIDModel::make(rET0_indexed_transitions_probabilities);

    GHMM::StatePtr rET0
            = GHMM::State::make(
                    state_indices["rET0"],
                    rcds_model,
                    rET0_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_final_duration,
                                                                                                 5, 0, 2, 3)));

    //State rET1 definition
    auto rET1_probabilities = vector<Probability>{{1}};
    auto rET1_probabilities_indices = vector<unsigned int> {state_indices["racc1"]};
    auto rET1_indexed_transitions_probabilities = index_probabilities(rET1_probabilities_indices, rET1_probabilities);

    print_probabilities(rET1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rET1_transitions_probabilities
            = DiscreteIIDModel::make(rET1_indexed_transitions_probabilities);

    GHMM::StatePtr rET1
            = GHMM::State::make(
                    state_indices["rET1"],
                    rcds_model,
                    rET1_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_final_duration,
                                                                                                 5, 1, 2, 3)));

    //State rET2 definition
    auto rET2_probabilities = vector<Probability>{{1}};
    auto rET2_probabilities_indices = vector<unsigned int> {state_indices["racc2"]};
    auto rET2_indexed_transitions_probabilities = index_probabilities(rET2_probabilities_indices, rET2_probabilities);

    print_probabilities(rET2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rET2_transitions_probabilities
            = DiscreteIIDModel::make(rET2_indexed_transitions_probabilities);

    GHMM::StatePtr rET2
            = GHMM::State::make(
                    state_indices["rET2"],
                    rcds_model,
                    rET2_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_final_duration,
                                                                                                 5, 2, 2, 3)));

    //State rEI0 definition
    auto rEI0_probabilities = vector<Probability>{{1}};
    auto rEI0_probabilities_indices = vector<unsigned int> {state_indices["rstart"]};
    auto rEI0_indexed_transitions_probabilities = index_probabilities(rEI0_probabilities_indices, rEI0_probabilities);

    print_probabilities(rEI0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rEI0_transitions_probabilities
            = DiscreteIIDModel::make(rEI0_indexed_transitions_probabilities);

    GHMM::StatePtr rEI0
            = GHMM::State::make(
                    state_indices["rEI0"],
                    rcds_model,
                    rEI0_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_initial_duration,
                                                                                                 11, 0, 0, 3)));

    //State rEI1 definition
    auto rEI1_probabilities = vector<Probability>{{1}};
    auto rEI1_probabilities_indices = vector<unsigned int> {state_indices["rstart"]};
    auto rEI1_indexed_transitions_probabilities = index_probabilities(rEI1_probabilities_indices, rEI1_probabilities);

    print_probabilities(rEI1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rEI1_transitions_probabilities
            = DiscreteIIDModel::make(rEI1_indexed_transitions_probabilities);

    GHMM::StatePtr rEI1
            = GHMM::State::make(
                    state_indices["rEI1"],
                    rcds_model,
                    rEI1_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_initial_duration,
                                                                                                 11, 0, 1, 3)));

    //State rEI2 definition
    auto rEI2_probabilities = vector<Probability>{{1}};
    auto rEI2_probabilities_indices = vector<unsigned int> {state_indices["rstart"]};
    auto rEI2_indexed_transitions_probabilities = index_probabilities(rEI2_probabilities_indices, rEI2_probabilities);

    print_probabilities(rEI2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rEI2_transitions_probabilities
            = DiscreteIIDModel::make(rEI2_indexed_transitions_probabilities);

    GHMM::StatePtr rEI2
            = GHMM::State::make(
                    state_indices["rEI2"],
                    rcds_model,
                    rEI2_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_initial_duration,
                                                                                                 11, 0, 2, 3)));

    //State rES definition
    auto rES_probabilities = vector<Probability>{{1}};
    auto rES_probabilities_indices = vector<unsigned int> {state_indices["rstart"]};
    auto rES_indexed_transitions_probabilities = index_probabilities(rES_probabilities_indices, rES_probabilities);

    print_probabilities(rES_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rES_transitions_probabilities
            = DiscreteIIDModel::make(rES_indexed_transitions_probabilities);

    GHMM::StatePtr rES
            = GHMM::State::make(
                    state_indices["rES"],
                    rcds_model,
                    rES_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_single_duration,
                                                                                                 4, 0, 2, 3)));

    //State rE00 definition
    auto rE00_probabilities = vector<Probability>{{1}};
    auto rE00_probabilities_indices = vector<unsigned int> {state_indices["racc0"]};
    auto rE00_indexed_transitions_probabilities = index_probabilities(rE00_probabilities_indices, rE00_probabilities);

    print_probabilities(rE00_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rE00_transitions_probabilities
            = DiscreteIIDModel::make(rE00_indexed_transitions_probabilities);

    GHMM::StatePtr rE00
            = GHMM::State::make(
                    state_indices["rE00"],
                    rcds_model,
                    rE00_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 0, 3)));
    //State rE01 definition
    auto rE01_probabilities = vector<Probability>{{1}};
    auto rE01_probabilities_indices = vector<unsigned int> {state_indices["racc1"]};
    auto rE01_indexed_transitions_probabilities = index_probabilities(rE01_probabilities_indices, rE01_probabilities);

    print_probabilities(rE01_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rE01_transitions_probabilities
            = DiscreteIIDModel::make(rE01_indexed_transitions_probabilities);

    GHMM::StatePtr rE01
            = GHMM::State::make(
                    state_indices["rE01"],
                    rcds_model,
                    rE01_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 2, 3)));
    //State rE02 definition
    auto rE02_probabilities = vector<Probability>{{1}};
    auto rE02_probabilities_indices = vector<unsigned int> {state_indices["racc2"]};
    auto rE02_indexed_transitions_probabilities = index_probabilities(rE02_probabilities_indices, rE02_probabilities);

    print_probabilities(rE02_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rE02_transitions_probabilities
            = DiscreteIIDModel::make(rE02_indexed_transitions_probabilities);

    GHMM::StatePtr rE02
            = GHMM::State::make(
                    state_indices["rE02"],
                    rcds_model,
                    rE02_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 1, 3)));

    //State rE10 definition
    auto rE10_probabilities = vector<Probability>{{1}};
    auto rE10_probabilities_indices = vector<unsigned int> {state_indices["racc0"]};
    auto rE10_indexed_transitions_probabilities = index_probabilities(rE10_probabilities_indices, rE10_probabilities);

    print_probabilities(rE10_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rE10_transitions_probabilities
            = DiscreteIIDModel::make(rE10_indexed_transitions_probabilities);

    GHMM::StatePtr rE10
            = GHMM::State::make(
                    state_indices["rE10"],
                    rcds_model,
                    rE10_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 1, 3)));
    //State rE11 definition
    auto rE11_probabilities = vector<Probability>{{1}};
    auto rE11_probabilities_indices = vector<unsigned int> {state_indices["racc1"]};
    auto rE11_indexed_transitions_probabilities = index_probabilities(rE11_probabilities_indices, rE11_probabilities);

    print_probabilities(rE11_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rE11_transitions_probabilities
            = DiscreteIIDModel::make(rE11_indexed_transitions_probabilities);

    GHMM::StatePtr rE11
            = GHMM::State::make(
                    state_indices["rE11"],
                    rcds_model,
                    rE11_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 0, 3)));
    //State rE12 definition
    auto rE12_probabilities = vector<Probability>{{1}};
    auto rE12_probabilities_indices = vector<unsigned int> {state_indices["racc2"]};
    auto rE12_indexed_transitions_probabilities = index_probabilities(rE12_probabilities_indices, rE12_probabilities);

    print_probabilities(rE12_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rE12_transitions_probabilities
            = DiscreteIIDModel::make(rE12_indexed_transitions_probabilities);

    GHMM::StatePtr rE12
            = GHMM::State::make(
                    state_indices["rE12"],
                    rcds_model,
                    rE12_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 2, 3)));

    //State rE20 definition
    auto rE20_probabilities = vector<Probability>{{1}};
    auto rE20_probabilities_indices = vector<unsigned int> {state_indices["racc0"]};
    auto rE20_indexed_transitions_probabilities = index_probabilities(rE20_probabilities_indices, rE20_probabilities);

    print_probabilities(rE20_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rE20_transitions_probabilities
            = DiscreteIIDModel::make(rE20_indexed_transitions_probabilities);

    GHMM::StatePtr rE20
            = GHMM::State::make(
                    state_indices["rE20"],
                    rcds_model,
                    rE20_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 2, 3)));
    //State rE21 definition
    auto rE21_probabilities = vector<Probability>{{1}};
    auto rE21_probabilities_indices = vector<unsigned int> {state_indices["racc1"]};
    auto rE21_indexed_transitions_probabilities = index_probabilities(rE21_probabilities_indices, rE21_probabilities);

    print_probabilities(rE21_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rE21_transitions_probabilities
            = DiscreteIIDModel::make(rE21_indexed_transitions_probabilities);

    GHMM::StatePtr rE21
            = GHMM::State::make(
                    state_indices["rE21"],
                    rcds_model,
                    rE21_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 1, 3)));
    //State rE22 definition
    auto rE22_probabilities = vector<Probability>{{1}};
    auto rE22_probabilities_indices = vector<unsigned int> {state_indices["racc2"]};
    auto rE22_indexed_transitions_probabilities = index_probabilities(rE22_probabilities_indices, rE22_probabilities);

    print_probabilities(rE22_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rE22_transitions_probabilities
            = DiscreteIIDModel::make(rE22_indexed_transitions_probabilities);

    GHMM::StatePtr rE22
            = GHMM::State::make(
                    state_indices["rE22"],
                    rcds_model,
                    rE22_transitions_probabilities,
                    ExplicitDuration::make(PhasedRunLengthDistribution::makeFromDiscreteIIDModel(exon_internal_duration,
                                                                                                 12, 0, 0, 3)));
    //State rstart definition
    auto rstart_probabilities = vector<Probability>{{1}};
    auto rstart_probabilities_indices = vector<unsigned int> {state_indices["N"]};
    auto rstart_indexed_transitions_probabilities = index_probabilities(rstart_probabilities_indices, rstart_probabilities);

    print_probabilities(rstart_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rstart_transitions_probabilities
            = DiscreteIIDModel::make(rstart_indexed_transitions_probabilities);

    GHMM::StatePtr rstart
            = GHMM::State::make(
                    state_indices["rstart"],
                    rstart_model,
                    rstart_transitions_probabilities,
                    SignalDuration::make(27));

    //State rstop definition
    auto rstop_probabilities = vector<Probability>{{0.1, 0.3, 0.3, 0.3}};
    auto rstop_probabilities_indices = vector<unsigned int> {state_indices["rES"], state_indices["rET0"],
                                                             state_indices["rET1"], state_indices["rET2"]};
    auto rstop_indexed_transitions_probabilities = index_probabilities(rstop_probabilities_indices, rstop_probabilities);

    print_probabilities(rstop_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rstop_transitions_probabilities
            = DiscreteIIDModel::make(rstop_indexed_transitions_probabilities);

    GHMM::StatePtr rstop
            = GHMM::State::make(
                    state_indices["rstop"],
                    rstop_model,
                    rstop_transitions_probabilities,
                    SignalDuration::make(3));

    //State racc0 definition
    auto racc0_probabilities = vector<Probability>{{0.144160536105428, 0.855839463894572}};
    auto racc0_probabilities_indices = vector<unsigned int> {state_indices["rIf0"], state_indices["rIs0"]};
    auto racc0_indexed_transitions_probabilities = index_probabilities(racc0_probabilities_indices, racc0_probabilities);

    print_probabilities(racc0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr racc0_transitions_probabilities
            = DiscreteIIDModel::make(racc0_indexed_transitions_probabilities);

    GHMM::StatePtr racc0
            = GHMM::State::make(
                    state_indices["racc0"],
                    racceptor_model,
                    racc0_transitions_probabilities,
                    SignalDuration::make(42));

    //State racc1 definition
    auto racc1_probabilities = vector<Probability>{{0.144160536105428, 0.855839463894572}};
    auto racc1_probabilities_indices = vector<unsigned int> {state_indices["rIf1"], state_indices["rIs1"]};
    auto racc1_indexed_transitions_probabilities = index_probabilities(racc1_probabilities_indices, racc1_probabilities);

    print_probabilities(racc1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr racc1_transitions_probabilities
            = DiscreteIIDModel::make(racc1_indexed_transitions_probabilities);

    GHMM::StatePtr racc1
            = GHMM::State::make(
                    state_indices["racc1"],
                    racceptor_model,
                    racc1_transitions_probabilities,
                    SignalDuration::make(42));

    //State racc2 definition
    auto racc2_probabilities = vector<Probability>{{0.144160536105428, 0.855839463894572}};
    auto racc2_probabilities_indices = vector<unsigned int> {state_indices["rIf2"], state_indices["rIs2"]};
    auto racc2_indexed_transitions_probabilities = index_probabilities(racc2_probabilities_indices, racc2_probabilities);

    print_probabilities(racc2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr racc2_transitions_probabilities
            = DiscreteIIDModel::make(racc2_indexed_transitions_probabilities);

    GHMM::StatePtr racc2
            = GHMM::State::make(
                    state_indices["racc2"],
                    racceptor_model,
                    racc2_transitions_probabilities,
                    SignalDuration::make(42));

    //State rdon0 definition
    auto rdon0_probabilities = vector<Probability>{{0.3, 0.3, 0.3, 0.1}};
    auto rdon0_probabilities_indices = vector<unsigned int> {state_indices["rE20"], state_indices["rE21"],
                                                             state_indices["rE22"], state_indices["rEI2"]};
    auto rdon0_indexed_transitions_probabilities = index_probabilities(rdon0_probabilities_indices, rdon0_probabilities);

    print_probabilities(rdon0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rdon0_transitions_probabilities
            = DiscreteIIDModel::make(rdon0_indexed_transitions_probabilities);

    GHMM::StatePtr rdon0
            = GHMM::State::make(
                    state_indices["rdon0"],
                    rdonor_model,
                    rdon0_transitions_probabilities,
                    SignalDuration::make(13));

    //State rdon1 definition
    auto rdon1_probabilities = vector<Probability>{{0.3, 0.3, 0.3, 0.1}};
    auto rdon1_probabilities_indices = vector<unsigned int> {state_indices["rE00"], state_indices["rE01"],
                                                             state_indices["rE02"], state_indices["rEI0"]};
    auto rdon1_indexed_transitions_probabilities = index_probabilities(rdon1_probabilities_indices, rdon1_probabilities);

    print_probabilities(rdon1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rdon1_transitions_probabilities
            = DiscreteIIDModel::make(rdon1_indexed_transitions_probabilities);

    GHMM::StatePtr rdon1
            = GHMM::State::make(
                    state_indices["rdon1"],
                    rdonor_model,
                    rdon1_transitions_probabilities,
                    SignalDuration::make(13));

    //State rdon2 definition
    auto rdon2_probabilities = vector<Probability>{{0.3, 0.3, 0.3, 0.1}};
    auto rdon2_probabilities_indices = vector<unsigned int> {state_indices["rE10"], state_indices["rE11"],
                                                             state_indices["rE12"], state_indices["rEI1"]};
    auto rdon2_indexed_transitions_probabilities = index_probabilities(rdon2_probabilities_indices, rdon2_probabilities);

    print_probabilities(rdon2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rdon2_transitions_probabilities
            = DiscreteIIDModel::make(rdon2_indexed_transitions_probabilities);

    GHMM::StatePtr rdon2
            = GHMM::State::make(
                    state_indices["rdon2"],
                    rdonor_model,
                    rdon2_transitions_probabilities,
                    SignalDuration::make(13));

    //State rI0 definition
    auto rI0_probabilities = vector<Probability>{{0.998908491611056, 0.00109150838894445}};
    auto rI0_probabilities_indices = vector<Id> {state_indices["rI0"],
                                                 state_indices["rdon0"]};
    auto rI0_indexed_transitions_probabilities = index_probabilities(rI0_probabilities_indices,
                                                                     rI0_probabilities);

    print_probabilities(rI0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rI0_transitions_probabilities
            = DiscreteIIDModel::make(rI0_indexed_transitions_probabilities);

    GHMM::StatePtr rI0
            = GHMM::State::make(
                    state_indices["rI0"],
                    non_coding_model,
                    rI0_transitions_probabilities,
                    GeometricDuration::make(state_indices["rI0"],
                                            rI0_transitions_probabilities));

    //State rI1 definition
    auto rI1_probabilities = vector<Probability>{{0.998908491611056, 0.00109150838894445}};
    auto rI1_probabilities_indices = vector<unsigned int> {state_indices["rI1"], state_indices["rdon1"]};
    auto rI1_indexed_transitions_probabilities = index_probabilities(rI1_probabilities_indices, rI1_probabilities);

    print_probabilities(rI1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rI1_transitions_probabilities
            = DiscreteIIDModel::make(rI1_indexed_transitions_probabilities);

    GHMM::StatePtr rI1
            = GHMM::State::make(
                    state_indices["rI1"],
                    non_coding_model,
                    rI1_transitions_probabilities,
                    GeometricDuration::make(state_indices["rI1"],
                                            rI1_transitions_probabilities));

    //State rI2 definition
    auto rI2_probabilities = vector<Probability>{{0.998908491611056, 0.00109150838894445}};
    auto rI2_probabilities_indices = vector<unsigned int> {state_indices["rI2"], state_indices["rdon2"]};
    auto rI2_indexed_transitions_probabilities = index_probabilities(rI2_probabilities_indices, rI2_probabilities);

    print_probabilities(rI2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rI2_transitions_probabilities
            = DiscreteIIDModel::make(rI2_indexed_transitions_probabilities);

    GHMM::StatePtr rI2
            = GHMM::State::make(
                    state_indices["rI2"],
                    non_coding_model,
                    rI2_transitions_probabilities,
                    GeometricDuration::make(state_indices["rI2"],
                                            rI2_transitions_probabilities));

    //State rIs0 definition
    auto rIs0_probabilities = vector<Probability>{{1}};
    auto rIs0_probabilities_indices = vector<unsigned int> {state_indices["rdon0"]};
    auto rIs0_indexed_transitions_probabilities = index_probabilities(rIs0_probabilities_indices, rIs0_probabilities);

    print_probabilities(rIs0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rIs0_transitions_probabilities
            = DiscreteIIDModel::make(rIs0_indexed_transitions_probabilities);

    GHMM::StatePtr rIs0
            = GHMM::State::make(
                    state_indices["rIs0"],
                    non_coding_model,
                    rIs0_transitions_probabilities,
                    ExplicitDuration::make(intron_short_duration));

    //State rIs1 definition
    auto rIs1_probabilities = vector<Probability>{{1}};
    auto rIs1_probabilities_indices = vector<unsigned int> {state_indices["rdon1"]};
    auto rIs1_indexed_transitions_probabilities = index_probabilities(rIs1_probabilities_indices, rIs1_probabilities);

    print_probabilities(rIs1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rIs1_transitions_probabilities
            = DiscreteIIDModel::make(rIs1_indexed_transitions_probabilities);

    GHMM::StatePtr rIs1
            = GHMM::State::make(
                    state_indices["rIs1"],
                    non_coding_model,
                    rIs1_transitions_probabilities,
                    ExplicitDuration::make(intron_short_duration));

    //State rIs2 definition
    auto rIs2_probabilities = vector<Probability>{{1}};
    auto rIs2_probabilities_indices = vector<unsigned int> {state_indices["rdon2"]};
    auto rIs2_indexed_transitions_probabilities = index_probabilities(rIs2_probabilities_indices, rIs2_probabilities);

    print_probabilities(rIs2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rIs2_transitions_probabilities
            = DiscreteIIDModel::make(rIs2_indexed_transitions_probabilities);

    GHMM::StatePtr rIs2
            = GHMM::State::make(
                    state_indices["rIs2"],
                    non_coding_model,
                    rIs2_transitions_probabilities,
                    ExplicitDuration::make(intron_short_duration));

    //State rIf0 definition
    auto rIf0_probabilities = vector<Probability>{{1}};
    auto rIf0_probabilities_indices = vector<unsigned int> {state_indices["rI0"]};
    auto rIf0_indexed_transitions_probabilities = index_probabilities(rIf0_probabilities_indices, rIf0_probabilities);

    print_probabilities(rIf0_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rIf0_transitions_probabilities
            = DiscreteIIDModel::make(rIf0_indexed_transitions_probabilities);

    GHMM::StatePtr rIf0
            = GHMM::State::make(
                    state_indices["rIf0"],
                    non_coding_model,
                    rIf0_transitions_probabilities,
                    SignalDuration::make(584));

    //State rIf1 definition
    auto rIf1_probabilities = vector<Probability>{{1}};
    auto rIf1_probabilities_indices = vector<unsigned int> {state_indices["rI1"]};
    auto rIf1_indexed_transitions_probabilities = index_probabilities(rIf1_probabilities_indices, rIf1_probabilities);

    print_probabilities(rIf1_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rIf1_transitions_probabilities
            = DiscreteIIDModel::make(rIf1_indexed_transitions_probabilities);

    GHMM::StatePtr rIf1
            = GHMM::State::make(
                    state_indices["rIf1"],
                    non_coding_model,
                    rIf1_transitions_probabilities,
                    SignalDuration::make(584));

    //State rIf2 definition
    auto rIf2_probabilities = vector<Probability>{{1}};
    auto rIf2_probabilities_indices = vector<unsigned int> {state_indices["rI2"]};
    auto rIf2_indexed_transitions_probabilities = index_probabilities(rIf2_probabilities_indices, rIf2_probabilities);

    print_probabilities(rIf2_indexed_transitions_probabilities);

    DiscreteIIDModelPtr rIf2_transitions_probabilities
            = DiscreteIIDModel::make(rIf2_indexed_transitions_probabilities);

    GHMM::StatePtr rIf2
            = GHMM::State::make(
                    state_indices["rIf2"],
                    non_coding_model,
                    rIf2_transitions_probabilities,
                    SignalDuration::make(584));

    return 0;
}