/***********************************************************************/
/*  Copyright 2015 ToPS                                                */
/*                                                                     */
/*  This program is free software; you can redistribute it and/or      */
/*  modify it under the terms of the GNU  General Public License as    */
/*  published by the Free Software Foundation; either version 3 of     */
/*  the License, or (at your option) any later version.                */
/*                                                                     */
/*  This program is distributed in the hope that it will be useful,    */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of     */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      */
/*  GNU General Public License for more details.                       */
/*                                                                     */
/*  You should have received a copy of the GNU General Public License  */
/*  along with this program; if not, write to the Free Software        */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,         */
/*  MA 02110-1301, USA.                                                */
/***********************************************************************/

// Standard headers
#include <vector>
#include <map>
#include <string>
#include <cmath>

// ToPS headers
#include "model/DiscreteIIDModel.hpp"
#include "model/Random.hpp"

namespace tops {
namespace model {

DiscreteIIDModel::DiscreteIIDModel(std::vector<double> probabilities)
    : _probabilities(probabilities) {
}

DiscreteIIDModelPtr DiscreteIIDModel::make(std::vector<double> probabilities) {
  return DiscreteIIDModelPtr(new DiscreteIIDModel(probabilities));
}

DiscreteIIDModelPtr DiscreteIIDModel::trainML(
    std::vector<Sequence> training_set,
    unsigned int alphabet_size) {
  std::vector<double> probabilities(alphabet_size, 0);
  unsigned int number_of_symbols = 0;
  for (auto sequence : training_set) {
    for (auto symbol : sequence) {
      probabilities[symbol]++;
      number_of_symbols++;
    }
  }
  for (Symbol s = 0; s < alphabet_size; s++)
    probabilities[s] = log(probabilities[s]/number_of_symbols);
  return DiscreteIIDModel::make(probabilities);
}

DiscreteIIDModelPtr DiscreteIIDModel::trainSmoothedHistogramBurge(
    std::vector<Sequence> training_set,
    double c,
    unsigned int max_length) {

  std::vector<Symbol> data;

  for (auto sequence : training_set)
    for (auto symbol : sequence)
      data.push_back(symbol);

  std::map<Symbol, double> counter;
  std::map<Symbol, double> sum;
  std::map<Symbol, double>::const_iterator iter;

  for (auto symbol : data) {
    iter = counter.find(symbol);
    if (iter == counter.end())
      counter[symbol] = 1.0;
    else
      counter[symbol] += 1.0;
  }
  double total = 0.0;
  for (unsigned int k = 1; k <= max_length; k++) {
    int start = k - 10;
    int end = k + 10;
    if (start < 0)
      start = 0;
    sum[k] = 0.0;
    for (int x = start; x < end; x++) {
      iter = counter.find(x);
      if (iter != counter.end() && iter->second > 0.0) {
        double nx = iter->second;
        double mean = x+1.0;
        double sd = sqrt(2*(x+1.0)*c/nx);
        double px2 = 0.5*(1 + erf(((k+1.5) - mean))/ (sd*sqrt(2.0)));
        double px1 = 0.5*(1 + erf(((k+0.5) - mean))/ (sd*sqrt(2.0)));
        sum[k] += nx*(px2 - px1);
      }
    }
    sum[k] = sum[k]/data.size();
    total = total+ sum[k];
  }
  double epsilon = 1e-5;
  total = total/(1 - max_length*epsilon);

  std::vector<double> prob;
  prob.resize(max_length+1);
  for (int k = 1; k <= max_length; k++) {
    prob[k] = epsilon + sum[k]/total;
  }

  return DiscreteIIDModel::make(prob);
}

DiscreteIIDModelPtr DiscreteIIDModel::trainSmoothedHistogramStanke(
      std::vector<Sequence> training_set,
      std::vector<unsigned int> weights,
      unsigned int max_length,
      int m,
      double slope) {

  double a = slope;
  unsigned int max = max_length;
  unsigned int L = max;
  max = max + 4 * a * max;

  std::vector<Symbol> data;
  for (unsigned int i = 0; i < training_set.size(); i++)
    for (auto symbol : training_set[i])
      for (unsigned int k = 0; k < weights[i]; k++)
        data.push_back(symbol);

  std::map<Symbol, int> counter;
  std::vector<double> prob;


  std::vector<double> pi;
  pi.resize(L);

  if (data.size() > 0) {
    for(unsigned int i = 0; i < data.size(); i++){
      if(counter.find(data[i]) == counter.end())
        counter[data[i]] = 1.0;
      else
        counter[data[i]] += 1.0;
    }

    double count_left = 0;
    double count_right = 0;

    for (int pos = 0; (pos < L) && (pos < max) ; pos +=1) {
      int bwd = (int) (.01 + (a / pow(L, 1.0/5.0) ) * (double) pos);
      if(bwd <= 0)
        bwd = 1;
      for(int j = pos - bwd + 1;  (j <= pos + bwd -1)  ; j++) {
        if (! (j >= 0 && j < L))
          continue;
        if(j <= pos)
          count_left += (counter[j]) ? 1: 0;
        if(j >= pos)
          count_right += (counter[j])? 1: 0;
      }

      while (count_left < m && count_right < m && bwd < L) {
        bwd ++;
        if(pos + bwd -1 < L)
          count_left += counter[pos + bwd - 1] ? 1:0;
        if(pos - bwd + 1 >= 0)
          count_right += counter[pos + bwd - 1] ? 1:0;
      }

      if(pos < L)
        pi[pos] += kernel_normal((double)0, (double)bwd) * counter[pos];
      bool negligible = false;
      int j=1;
      while (!negligible && (pos-j>=0 || pos+j<L)){
        double  wj = kernel_normal(j, bwd) * (counter[pos] );
        if (pos-j>=0 && pos-j<(int)pi.size() ) {
          pi[pos-j] += wj;
        }
        if (pos+j<(int)pi.size() && pos+j>=0) {
          pi[pos+j] += wj;
        }
        negligible = (wj < 1e-20);
        j++;
      }
    }

    double total = 0;
    for (long k = 0; k < (int)pi.size(); k++)
      total += pi[k];
    prob.resize(L);
    for (long k = 0; k < (int)pi.size(); k++) {
      prob[k] =  pi[k]/(total) ;
    }
  }

  return DiscreteIIDModel::make(prob);
}

double DiscreteIIDModel::kernel_normal(double x, double h) {
  double y = (x/h) * (x/h) ;
  double f = 1.0/(sqrt(2*M_PI));
  double v =  (f/h) * exp (- y/2);
  return v;
}

int DiscreteIIDModel::alphabetSize() const {
  return _probabilities.size();
}

double DiscreteIIDModel::probabilityOf(Symbol s) const {
  return _probabilities[s];
}

double DiscreteIIDModel::evaluatePosition(const Sequence &s,
                                          unsigned int i) const {
  return _probabilities[s[i]];
}

Symbol DiscreteIIDModel::choosePosition(const Sequence &s,
                                        unsigned int i) const {
  double random = generateRandomDouble();
  for (unsigned int symbol = 0; symbol < _probabilities.size(); symbol++) {
    random -= exp(_probabilities[symbol]);
    if (random <= 0)
      return symbol;
  }
  return _probabilities.size()-1;
}


}  // namespace model
}  // namespace tops
