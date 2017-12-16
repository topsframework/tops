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
                    GeometricDuration::make(state_indices["Is0"],
                                            Is0_transitions_probabilities));

    return 0;
}