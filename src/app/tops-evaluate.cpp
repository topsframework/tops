// Standard headers
#include <string>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <exception>

// Internal headers
#include "model/DiscreteIIDModel.hpp"
#include "model/HiddenMarkovModel.hpp"
#include "model/GeneralizedHiddenMarkovModel.hpp"
#include "model/GeneralizedHiddenMarkovModelState.hpp"
#include "model/VariableLengthMarkovChain.hpp"
#include "model/InhomogeneousMarkovChain.hpp"
#include "model/PeriodicInhomogeneousMarkovChain.hpp"
#include "model/MultipleSequentialModel.hpp"
#include "model/SimilarityBasedSequenceWeighting.hpp"

#include "model/Probability.hpp"
#include "model/ProbabilisticModel.hpp"
#include "model/ProbabilisticModelCrtp.hpp"
#include "model/Symbol.hpp"

#include "model/Sequence.hpp"
#include "model/ContextTreeNode.hpp"
#include "model/FixedDuration.hpp"
#include "model/ExplicitDuration.hpp"

#include "config/BasicConfig.hpp"
#include "config/Config.hpp"
#include "config/Converter.hpp"
#include "config/DiscreteConverter.hpp"
#include "config/Domain.hpp"
#include "config/state/StateConfig.hpp"

#include "config/duration/DurationConfig.hpp"
#include "config/duration/ExplicitDurationConfig.hpp"
#include "config/duration/FixedDurationConfig.hpp"
#include "config/duration/GeometricDurationConfig.hpp"
#include "config/duration/MaxLengthDurationConfig.hpp"

#include "config/model/definition/ModelConfig.hpp"
#include "config/model/definition/IIDConfig.hpp"
#include "config/model/definition/HMMConfig.hpp"
#include "config/model/definition/GHMMConfig.hpp"
#include "config/model/definition/VLMCConfig.hpp"
#include "config/model/definition/IMCConfig.hpp"
#include "config/model/definition/PeriodicIMCConfig.hpp"
#include "config/model/definition/MSMConfig.hpp"
#include "config/model/definition/SBSWConfig.hpp"

#include "lang/Interpreter.hpp"
#include "lang/ModelConfigSerializer.hpp"

#include "chaiscript/dispatchkit/bootstrap_stl.hpp"

/*
TEST
*/
#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#   include <cxxabi.h>
#endif
#include <memory>
#include <string>
#include <cstdlib>

template <class T>
std::string
type_name() {
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void(*)(void*)> own
           (
#ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                           nullptr, nullptr),
#else
                nullptr,
#endif
                std::free
           );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}
// -----------------------------

// Using declarations
using tops::config::operator""_t;

struct GeneratedReturn {
    tops::model::ProbabilisticModelPtr model;
    tops::config::ConverterPtr converter;
};

std::vector<unsigned long int> get_sequence(std::string context, tops::config::ConverterPtr converter);

// Model generator
GeneratedReturn generateModel(const tops::config::definition::ModelConfigPtr &cfg);
// Specific model generator
GeneratedReturn generateIID(const tops::config::definition::ModelConfigPtr &cfg);
GeneratedReturn generateHMM(const tops::config::definition::ModelConfigPtr &cfg);
GeneratedReturn generateGHMM(const tops::config::definition::ModelConfigPtr &cfg);
GeneratedReturn generateVLMC(const tops::config::definition::ModelConfigPtr &cfg);
GeneratedReturn generateIMC(const tops::config::definition::ModelConfigPtr &cfg);
GeneratedReturn generatePeriodicIMC(const tops::config::definition::ModelConfigPtr &cfg);
GeneratedReturn generateMSM(const tops::config::definition::ModelConfigPtr &cfg);

GeneratedReturn generateSBSW(const tops::config::definition::ModelConfigPtr &cfg);

int usage(const std::string &program_name) {
    std::cerr << "USAGE: " << program_name
              << " model_config [dataset] [output_dir]" << std::endl;
    return EXIT_FAILURE;
}

int main(int argc, char **argv) try {
    if (argc < 1)
        return usage(argv[0]);

    //std::string config_type = "example/models/iid.tops";
    std::string config_type = argv[1];
    //std::string file = "data/cassino.tsv";
    std::string file = argv[2];

    tops::lang::Interpreter interpreter;
    auto cfg = interpreter.evalModelDefinition(config_type);

    GeneratedReturn generated_return = generateModel(cfg);
    auto [model, converter] = generated_return;

    std::fstream dataset(file);
    std::string line;

    // Header
    std::getline(dataset, line);
    std::cerr << line << std::endl;

    // Data
    std::vector<tops::model::Symbol> data;
    while (std::getline(dataset, line))
    {
        std::stringstream ss(line);

        std::string input;
        std::getline(ss, input, '\t');

        std::cerr << input << " - " << converter->convert(input) << std::endl;
        data.push_back(converter->convert(input));
    }

    auto evaluator = model->standardEvaluator({ data });
    auto full_probability = evaluator->evaluateSequence(0, data.size());

    std::cout << full_probability << std::endl;

    return 0;
}
catch (chaiscript::exception::eval_error &e) {
    std::cerr << e.pretty_print() << std::endl;
}
/*catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
}*/

GeneratedReturn generateModel(const tops::config::definition::ModelConfigPtr &cfg) {

    auto &model_type = std::get<decltype("model_type"_t)>(*cfg);

    if (model_type == "IID") {
        return generateIID(cfg);
    }
    else if (model_type == "HMM") {
        return generateHMM(cfg);
    }
    else if (model_type == "GHMM") {
        return generateGHMM(cfg);
    }
    else if (model_type == "VLMC") {
        return generateVLMC(cfg);
    }
    else if (model_type == "IMC") {
        return generateIMC(cfg);
    }
    else if (model_type == "PeriodicIMC") {
        return generatePeriodicIMC(cfg);
    }
    else if (model_type == "MSM") {
        return generateMSM(cfg);
    }
    else if (model_type == "SBSW") {
        return generateSBSW(cfg);
    }
    else {
        std::cerr << "Model " << model_type << " does not exist!";
        return {};
    }
}

// gets the code of each symbol in a sequence
std::vector<unsigned long int> get_sequence(std::string context, tops::config::ConverterPtr converter) {
    std::vector<unsigned long int> sequence;
    std::string symbol = "";
    unsigned long int i = 0, l = context.length();
    while(i < l){
        if(context[i] != ' '){
            symbol += context[i++];
        }
        else{
            sequence.push_back(converter->convert(symbol));
            symbol = "";
            i++;
        }
    }
    sequence.push_back(converter->convert(symbol));
    return sequence;
}

//generates an IID model
GeneratedReturn generateIID(const tops::config::definition::ModelConfigPtr &cfg) {
    // downcast
    auto iid_cfg = std::dynamic_pointer_cast<tops::config::definition::IIDConfig>(cfg);

    auto &domains = std::get<decltype("observations"_t)>(*iid_cfg);
    auto converter = domains->makeConverter();

    //std::cout << type_name<decltype(*domain->data())>() << std::endl;
    //std::cout << type_name<decltype(discrete_converter)>() << std::endl;

    auto &emission_probabilities = std::get<decltype("emission_probabilities"_t)>(*iid_cfg);
    tops::model::Probabilities probies(emission_probabilities.size());

    std::cout << typeid(emission_probabilities).name() << std::endl;

    for (const auto &[symbol, probability] : emission_probabilities)
    {
        std::cerr << converter->convert(symbol) << ":";
        std::cerr << probability << " ";
        probies[converter->convert(symbol)] = probability;
    }

    std::cerr << std::endl;

    auto model = tops::model::DiscreteIIDModel::make(probies);

    return {model, converter};
}

// generates a HMM
GeneratedReturn generateHMM(const tops::config::definition::ModelConfigPtr &cfg) {
    // downcast
    auto hmm_cfg = std::dynamic_pointer_cast<tops::config::definition::HMMConfig>(cfg);

    auto &labels = std::get<decltype("labels"_t)>(*hmm_cfg);
    auto label_converter = labels->makeConverter();
    auto discrete_convert = std::dynamic_pointer_cast<tops::config::DiscreteConverter>(label_converter);
    auto state_alphabet_size = 0;
    if (discrete_convert)
    {
        state_alphabet_size = discrete_convert->alphabet().size();
    }

    auto &observations = std::get<decltype("observations"_t)>(*hmm_cfg);
    auto observations_converter = observations->makeConverter();
    auto discrete_convert_observations = std::dynamic_pointer_cast<tops::config::DiscreteConverter>(observations_converter);
    auto observation_alphabet_size = 0;
    if (discrete_convert_observations)
    {
        observation_alphabet_size = discrete_convert_observations->alphabet().size();
    }

    auto &emission_probabilities = std::get<decltype("emission_probabilities"_t)>(*hmm_cfg);

    tops::model::Probabilities state_emissions(observation_alphabet_size);
    //Create vector States
    std::vector<tops::model::HiddenMarkovModel::StatePtr> states(state_alphabet_size);

    // for maior do Label
    for (const auto &label : discrete_convert->alphabet())
    {
        //std::cout << label << " ";
        //auto label_symbol = label;
        auto label_symbol = label_converter->convert(label);

        // Ler emissões
        for (const auto &[key, probability] : emission_probabilities)
        {

            //"1" | "Fair"   : 1.0/6,
            std::istringstream iss(key);
            std::string observation;
            std::string pipe;
            std::string current_label;

            iss >> observation >> pipe >> current_label;

            auto current_label_symbol = label_converter->convert(current_label);

            //std::cout << current_label_symbol << " ";

            if (current_label_symbol == label_symbol)
            {
                std::cerr << "->" << label_symbol << " - " << probability << std::endl;
                auto observation_symbol = observations_converter->convert(observation);
                state_emissions[observation_symbol] = probability;
            }
        }

        //create IID Emission
        std::cerr << "emissions" << std::endl;
        for(size_t x=0; x<state_emissions.size(); x++){
            std::cerr << x << " - " << state_emissions[x] << std::endl;
        }

        auto model_emission = tops::model::DiscreteIIDModel::make(state_emissions);

        //Transition
        auto &transition_probabilities = std::get<decltype("transition_probabilities"_t)>(*hmm_cfg);
        //Create vector
        tops::model::Probabilities state_transition(state_alphabet_size);

        for (const auto &[key, probability] : transition_probabilities)
        {

            //fig = "Loaded" | "Fair" : 0.1,

            std::istringstream iss(key);
            std::string target_label;
            std::string pipe;
            std::string origin_label;

            iss >> target_label >> pipe >> origin_label;
            auto target_label_symbol = label_converter->convert(target_label);
            auto origin_label_symbol = label_converter->convert(origin_label);

            if (origin_label_symbol == label_symbol)
            {
                state_transition[target_label_symbol] = probability;
            }
        }
        //create IID Transition
        std::cerr << "transitions" << std::endl;
        for(size_t x=0; x<state_transition.size(); x++){
            std::cerr << x << " - " << state_transition[x] << std::endl;
        }

        auto model_transition = tops::model::DiscreteIIDModel::make(state_transition);

        // create States

        states[label_symbol] = tops::model::HiddenMarkovModel::MatchState::make(label_symbol, model_emission, model_transition);
    }

    std::cerr << std::endl;

    //auto model = tops::model::HiddenMarkovModel::make(states, state_alphabet_size, observation_alphabet_size);
    auto model = std::make_shared<tops::model::HiddenMarkovModel>(states, state_alphabet_size, observation_alphabet_size);

    return {model, observations_converter};
}

// generates a GHMM model
GeneratedReturn generateGHMM(const tops::config::definition::ModelConfigPtr &cfg) {

    auto ghmm_cfg = std::dynamic_pointer_cast<tops::config::definition::GHMMConfig>(cfg);

    auto &labels = std::get<decltype("labels"_t)>(*ghmm_cfg);
    auto label_converter = std::dynamic_pointer_cast<tops::config::DiscreteConverter>(labels->makeConverter());
    auto state_alphabet_size = 0;
    if (label_converter)
    {
        state_alphabet_size = label_converter->alphabet().size();
    }

    auto &observations = std::get<decltype("observations"_t)>(*ghmm_cfg);
    auto observations_converter = std::dynamic_pointer_cast<tops::config::DiscreteConverter>(observations->makeConverter());
    auto observation_alphabet_size = 0;
    if (observations_converter)
    {
        observation_alphabet_size = observations_converter->alphabet().size();
    }

    auto &states_cfg = std::get<decltype("states"_t)>(*ghmm_cfg);

    std::vector<tops::model::GeneralizedHiddenMarkovModel::StatePtr> states(states_cfg.size());
    std::vector<tops::model::DiscreteIIDModelPtr> transitions(states_cfg.size());

    auto &transition_probabilities = std::get<decltype("transition_probabilities"_t)>(*ghmm_cfg);

    for (const auto &label : label_converter->alphabet())
    {
        auto label_symbol = label_converter->convert(label);

        tops::model::Probabilities state_transition(states.size());
        for (const auto &[key, probability] : transition_probabilities)
        {
            std::istringstream iss(key);
            std::string target_label;
            std::string pipe;
            std::string origin_label;

            iss >> target_label >> pipe >> origin_label;
            auto target_label_symbol = label_converter->convert(target_label);
            auto origin_label_symbol = label_converter->convert(origin_label);

            if (origin_label_symbol == label_symbol)
            {
                state_transition[target_label_symbol] = probability;
            }
        }

        transitions[label_symbol] = tops::model::DiscreteIIDModel::make(state_transition);
    }

    for (const auto &[label, state_cfg] : states_cfg)
    {
        auto label_symbol = label_converter->convert(label);

        auto &duration_cfg = std::get<decltype("duration"_t)>(*state_cfg);
        auto &emission_cfg = std::get<decltype("emission"_t)>(*state_cfg);

        auto duration_fix_cfg = std::dynamic_pointer_cast<tops::config::FixedDurationConfig>(duration_cfg);
        auto duration_exp_cfg = std::dynamic_pointer_cast<tops::config::ExplicitDurationConfig>(duration_cfg);
        auto duration_geo_cfg = std::dynamic_pointer_cast<tops::config::GeometricDurationConfig>(duration_cfg);
        auto duration_max_cfg = std::dynamic_pointer_cast<tops::config::MaxLengthDurationConfig>(duration_cfg);

        tops::model::DurationPtr duration;

        if (duration_fix_cfg != nullptr)
        {

            auto &size = std::get<decltype("size"_t)>(*duration_fix_cfg);
            duration = tops::model::FixedDuration::make(size);
        }
        else if (duration_exp_cfg != nullptr)
        {
        }
        //...

        GeneratedReturn generated_return = generateModel(std::dynamic_pointer_cast<tops::config::definition::ModelConfig>(emission_cfg));

        auto emission = generated_return.model;

        states[label_symbol] = tops::model::GeneralizedHiddenMarkovModel::MatchState::make(label_symbol, emission, transitions[label_symbol], duration);
    }

    std::size_t num_phases = std::get<decltype("num_phases"_t)>(*ghmm_cfg);
    std::size_t max_backtracking = std::get<decltype("max_backtracking"_t)>(*ghmm_cfg);

    auto model = tops::model::GeneralizedHiddenMarkovModel::make(states, state_alphabet_size, observation_alphabet_size, num_phases, max_backtracking);

    return {std::static_pointer_cast<tops::model::ProbabilisticModel>(model), observations_converter};
}

//generates a VLMC model
GeneratedReturn generateVLMC(const tops::config::definition::ModelConfigPtr &cfg) {
    // downcast
    auto vlmc_cfg = std::dynamic_pointer_cast<tops::config::definition::VLMCConfig>(cfg);

    auto &domain = std::get<decltype("observations"_t)>(*vlmc_cfg);
    auto converter = domain->makeConverter();

    auto discrete_converter = std::dynamic_pointer_cast<tops::config::DiscreteConverter>(converter);
    int alphabet_size = 0;
    if (discrete_converter) {
        alphabet_size = discrete_converter->alphabet().size();
    }

    auto &context_probabilities = std::get<decltype("context_probabilities"_t)>(*vlmc_cfg);

    // create a map<context, vector<probabilities>> M and add each context to the context_tree in an ordered way
    std::map< std::string, tops::model::Probabilities> contexts_aux;
    std::map< std::string, tops::model::Probabilities>::iterator it;
    std::pair< std::map<std::string, tops::model::Probabilities>::iterator, bool> ptr_it;

    std::string s_symbol, s_context;
    unsigned long int l;
    for (const auto &[symbol_context, probability] : context_probabilities) {
        std::istringstream ss_aux(symbol_context);
        std::getline(ss_aux, s_symbol, '|');
        std::getline(ss_aux, s_context, '|');
        s_symbol.erase(s_symbol.end()-1);
        s_context.erase(s_context.begin());

        it = contexts_aux.find(s_context);
        if(it != contexts_aux.end()) {
            it->second[converter->convert(s_symbol)] = probability;
        }
        else {
            tops::model::Probabilities probability_context(alphabet_size);
            ptr_it = contexts_aux.insert(std::make_pair(s_context, probability_context));
            ptr_it.first->second[converter->convert(s_symbol)] = probability;
        }
    }

    auto context_tree = tops::model::ContextTree::make(alphabet_size);
    it = contexts_aux.begin();

    auto node = context_tree->createContext();
    node->setDistribution(tops::model::DiscreteIIDModel::make(it->second));

    it++;
    for (; it!=contexts_aux.end(); ++it){
        std::vector<unsigned long int> sequence = get_sequence(it->first, converter);
        l = sequence.size();

        tops::model::ContextTreeNodePtr parent;
        if(l > 1) {
            parent = context_tree->getContext(sequence, l - 1);
        }
        else {
            parent = context_tree->getContext(0);
        }

        //std::cout << "[" << it->first << "]\n";
        //std::cout << "p{" << parent->symbol() << "}\n";
        //parent->print(0);

        node = context_tree->createContext();
        node->setDistribution(tops::model::DiscreteIIDModel::make(it->second));

        parent->setChild(node, sequence[l - 1]);
    }

    auto model = tops::model::VariableLengthMarkovChain::make(context_tree);

    return {model, converter};
}

// generates an IMC model
GeneratedReturn generateIMC(const tops::config::definition::ModelConfigPtr &cfg) {
    // downcast
    auto imc_cfg = std::dynamic_pointer_cast<tops::config::definition::IMCConfig>(cfg);

    auto &domain = std::get<decltype("observations"_t)>(*imc_cfg);
    auto converter = domain->makeConverter();

    auto &position_specific_distributions = std::get<decltype("position_specific_distributions"_t)>(*imc_cfg);

    //std::cout << type_name<decltype(position_specific_distributions)>() << std::endl;

    std::vector<tops::model::VariableLengthMarkovChainPtr> vlmcs;
    tops::lang::Interpreter interpreter;

    for (const auto &vlmc_model : position_specific_distributions) {
        auto cfg_vlmc = interpreter.evalModelDefinition(vlmc_model->path());
        GeneratedReturn generated_vlmc = generateVLMC(cfg_vlmc);
        vlmcs.push_back(std::dynamic_pointer_cast<tops::model::VariableLengthMarkovChain>(generated_vlmc.model));
        //verify converter of each vlmc???
    }

    auto model = tops::model::InhomogeneousMarkovChain::make(vlmcs);

    return {model, converter};
}

// generates a Periodic IMC model
GeneratedReturn generatePeriodicIMC(const tops::config::definition::ModelConfigPtr &cfg) {
    // downcast
    auto periodic_imc_cfg = std::dynamic_pointer_cast<tops::config::definition::PeriodicIMCConfig>(cfg);

    auto &domain = std::get<decltype("observations"_t)>(*periodic_imc_cfg);
    auto converter = domain->makeConverter();

    auto &position_specific_distributions = std::get<decltype("position_specific_distributions"_t)>(*periodic_imc_cfg);

    std::vector<tops::model::VariableLengthMarkovChainPtr> vlmcs;
    tops::lang::Interpreter interpreter;

    for (const auto &vlmc_model : position_specific_distributions) {
        auto cfg_vlmc = interpreter.evalModelDefinition(vlmc_model->path());
        GeneratedReturn generated_vlmc = generateVLMC(cfg_vlmc);
        vlmcs.push_back(std::dynamic_pointer_cast<tops::model::VariableLengthMarkovChain>(generated_vlmc.model));
        //verify converter of each vlmc???
    }

    auto model = tops::model::PeriodicInhomogeneousMarkovChain::make(vlmcs);

    return {model, converter};
}

// generates a MSM model
GeneratedReturn generateMSM(const tops::config::definition::ModelConfigPtr &cfg) {
    // downcast
    auto msm_cfg = std::dynamic_pointer_cast<tops::config::definition::MSMConfig>(cfg);

    auto &domain = std::get<decltype("observations"_t)>(*msm_cfg);
    auto converter = domain->makeConverter();

    auto &msm_models = std::get<decltype("models"_t)>(*msm_cfg);

    std::vector<tops::model::ProbabilisticModelPtr> models;
    std::vector<int> max_lenghts;
    tops::lang::Interpreter interpreter;

    for (const auto &[label, emission_duration] : msm_models) {
        auto &emission_cfg = std::get<decltype("emission"_t)>(*emission_duration);
        auto &duration_cfg = std::get<decltype("duration"_t)>(*emission_duration);
        auto duration_max_cfg = std::dynamic_pointer_cast<tops::config::MaxLengthDurationConfig>(duration_cfg);
        auto &duration = std::get<decltype("size"_t)>(*duration_max_cfg);

        auto cfg_model = interpreter.evalModelDefinition(emission_cfg->path());
        GeneratedReturn generated_model = generateModel(cfg_model);

        models.push_back(generated_model.model);
        max_lenghts.push_back(duration);
    }

    auto model = tops::model::MultipleSequentialModel::make(models, max_lenghts);

    return {model, converter};
}

// generates a SBSW model
GeneratedReturn generateSBSW(const tops::config::definition::ModelConfigPtr &cfg) {
    auto sbsw_cfg = std::dynamic_pointer_cast<tops::config::definition::SBSWConfig>(cfg);

    auto &domain = std::get<decltype("observations"_t)>(*sbsw_cfg);
    auto converter = domain->makeConverter();

    auto &sequences = std::get<decltype("sequences"_t)>(*sbsw_cfg);

    for (const auto &[sequence, weight] : sequences) {
        std::cout << sequence << weight;
    }

    return {nullptr, nullptr};
}
