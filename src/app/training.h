//
// Created by bruno on 12/01/18.
//

#ifndef TOPS_TRAINING_H
#define TOPS_TRAINING_H
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

using Symbol = unsigned int;
using namespace std;

////------ Dataset Folder ------ ////
string DATASET = "dataset";
//// ---------------------- ////

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
            DATASET + "/" + model_name + "_duration/sequence.0.tsv",
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
    if ((dir = opendir((DATASET + "/noncoding").c_str()))!= NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence(DATASET + "/noncoding/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/rcds").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence(DATASET + "/rcds/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/cds").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence(DATASET + "/cds/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/rstop").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence(DATASET + "/rstop/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/stop").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence(DATASET + "/stop/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/rstart_initiation_motif").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence(DATASET + "/rstart_initiation_motif/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/rstart_initial_pattern").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset2.push_back(readSequence(DATASET + "/rstart_initial_pattern/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/start_initiation_motif").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence(DATASET + "/start_initiation_motif/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/start_initial_pattern").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset2.push_back(readSequence(DATASET + "/start_initial_pattern/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/rdonor").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence(DATASET + "/rdonor/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/rdonor_initial_pattern").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset2.push_back(readSequence(DATASET + "/rdonor_initial_pattern/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/donor").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence(DATASET + "/donor/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/donor_initial_pattern").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset2.push_back(readSequence(DATASET + "/donor_initial_pattern/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/racceptor_branch").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence(DATASET + "/racceptor_branch/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/racceptor").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset2.push_back(readSequence(DATASET + "/racceptor/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/racceptor_initial_pattern").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset3.push_back(readSequence(DATASET + "/racceptor_initial_pattern/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/acceptor_branch").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset.push_back(readSequence(DATASET + "/acceptor_branch/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/acceptor").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset2.push_back(readSequence(DATASET + "/acceptor/" + filename, {nucleotides})[0]);
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
    if ((dir = opendir((DATASET + "/acceptor_initial_pattern").c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto filename = std::string(ent->d_name);
            if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".tsv") {
                dataset3.push_back(readSequence(DATASET + "/acceptor_initial_pattern/" + filename, {nucleotides})[0]);
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

#endif //TOPS_TRAINING_H