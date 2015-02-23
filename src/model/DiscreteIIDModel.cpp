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
#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>

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

std::vector<double> DiscreteIIDModel::normalize(
    std::vector<double> probabilities) {
  double sum = 0;
  for (auto p : probabilities)
    sum += p;

  std::vector<double> log_probabilities;
  for (auto p : probabilities)
    log_probabilities.push_back(log(p/sum));

  return log_probabilities;
}

DiscreteIIDModelPtr DiscreteIIDModel::trainML(
    std::vector<Sequence> training_set,
    unsigned int alphabet_size) {
  std::vector<double> log_probabilities(alphabet_size, 0);
  unsigned int number_of_symbols = 0;
  for (auto sequence : training_set) {
    for (auto symbol : sequence) {
      log_probabilities[symbol]++;
      number_of_symbols++;
    }
  }
  for (Symbol s = 0; s < alphabet_size; s++)
    log_probabilities[s] = log(log_probabilities[s]/number_of_symbols);
  return DiscreteIIDModel::make(log_probabilities);
}

DiscreteIIDModelPtr DiscreteIIDModel::trainSmoothedHistogramBurge(
    std::vector<Sequence> training_set,
    double c,
    unsigned int max_length) {

  std::vector<Symbol> data;

  for (auto sequence : training_set)
    for (auto symbol : sequence)
      data.push_back(symbol);

  if (data.size() == 0)
    return DiscreteIIDModel::make(std::vector<double>{});

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
  for (unsigned int k = 0; k < max_length; k++) {
    unsigned int start = (k >= 10) ? (k - 10) : 0;
    unsigned int end = (k + 10 < max_length) ? k + 10 : max_length - 1;

    sum[k] = 0.0;
    for (unsigned int x = start; x < end; x++) {
      iter = counter.find(x);
      if (iter != counter.end() && iter->second > 0.0) {
        double nx = iter->second;
        double mean = x + 1.0;
        double sd = sqrt(2*(x+1.0)*c/nx);
        double px2 = 0.5*(1 + erf(((k+1.5) - mean))/ (sd*sqrt(2.0)));
        double px1 = 0.5*(1 + erf(((k+0.5) - mean))/ (sd*sqrt(2.0)));
        sum[k] += nx * (px2 - px1);
      }
    }
    sum[k] /= data.size();
    total += sum[k];
  }
  double epsilon = 1e-5;
  total /= 1 - max_length*epsilon;

  std::vector<double> prob(max_length);
  for (unsigned int k = 0; k < max_length; k++) {
    prob[k] = epsilon + sum[k]/total;
  }

  return DiscreteIIDModel::make(normalize(prob));
}

DiscreteIIDModelPtr DiscreteIIDModel::trainSmoothedHistogramStanke(
      std::vector<Sequence> training_set,
      std::vector<unsigned int> weights,
      unsigned int max_length,
      unsigned int m,
      double slope) {
  unsigned int L = max_length;
  unsigned int max = L + 4 * slope * L;

  std::vector<Symbol> data;
  for (unsigned int i = 0; i < training_set.size(); i++)
    for (auto symbol : training_set[i])
      for (unsigned int k = 0; k < weights[i]; k++)
        data.push_back(symbol);

  if (data.size() == 0)
    return DiscreteIIDModel::make(std::vector<double>{});

  std::map<Symbol, unsigned int> counter;
  std::vector<double> prob(L);

  for (unsigned int i = 0; i < data.size(); i++) {
    if (counter.find(data[i]) == counter.end())
      counter[data[i]] = 1;
    else
      counter[data[i]] += 1;
  }

  unsigned int count_left = 0;
  unsigned int count_right = 0;

  for (unsigned int pos = 0; (pos < L) && (pos < max); pos++) {
    double tmp = std::floor(0.01 + slope/pow(L, 1.0/5.0) * pos);
    unsigned int bwd = (tmp <= 0) ? 1 : static_cast<unsigned int>(tmp);

    for (unsigned int j = pos - bwd + 1; j <= pos + bwd -1; j++) {
      if (j >= L)
        continue;
      if (j <= pos)
        count_left += (counter[j]) ? 1 : 0;
      if (j >= pos)
        count_right += (counter[j]) ? 1 : 0;
    }

    while (count_left < m && count_right < m && bwd < L) {
      bwd++;
      if (pos < L - bwd  + 1)
        count_left += counter[pos + bwd - 1] ? 1:0;
      if (pos >= bwd - 1)
        count_right += counter[pos + bwd - 1] ? 1:0;
    }

    if (pos < L)
      prob[pos] += kernel_normal(0.0, bwd) * counter[pos];
    bool negligible = false;
    unsigned int j = 1;

    while (!negligible && (pos >= j || pos+j < L)) {
      double  wj = kernel_normal(j, bwd) * counter[pos];
      if (pos >= j && pos-j < prob.size()) {
        prob[pos-j] += wj;
      }
      if (pos+j < prob.size()) {
        prob[pos+j] += wj;
      }
      negligible = (wj < 1e-20);
      j++;
    }
  }

  double total = 0;
  for (auto& p : prob)
    total += p;
  for (auto& p : prob)
    p /= total;

  return DiscreteIIDModel::make(normalize(prob));
}

DiscreteIIDModelPtr DiscreteIIDModel::trainSmoothedHistogramKernelDensity(
      std::vector<Sequence> training_set,
      unsigned int max_length) {
  std::vector<double> data;
  for (auto sequence : training_set) {
    for (auto symbol : sequence) {
      data.push_back(symbol);
    }
  }

  if (data.size() == 0)
    return DiscreteIIDModel::make(std::vector<double>{});

  std::vector<double> prob(max_length, 0.0);
  double total = 0.0;

  double bandwidth = sj_bandwidth(data);

  for (unsigned int pos = 0; pos < max_length; pos++) {
    double min = kernel_density_estimation(pos-0.5, bandwidth, data);
    double max = kernel_density_estimation(pos+0.5, bandwidth, data);

    if (max < min)
      std::swap(min, max);

    prob[pos] = min + (max - min)/2;
    total += prob[pos];
  }

  for (auto& p : prob)
    p /= total;

  return DiscreteIIDModel::make(normalize(prob));
}

double DiscreteIIDModel::kernel_normal(double x, double h) {
  double y = (x/h) * (x/h);
  double f = 1.0/(sqrt(2*M_PI));
  double v = (f/h) * exp(-y/2);
  return v;
}

double DiscreteIIDModel::epanechnikov(double x, double h) {
  double a = h * sqrt(5.0);
  double absx = fabs(x);
  if (absx < a) {
    return (3.0/4.0) * (1.0 - (absx/a)*(absx/a))/a;
  } else {
    return 0.0;
  }
}

#define abs9(a) (a > 0 ? a:-a)

void DiscreteIIDModel::band_den_bin(int n,
                                    int nb,
                                    double *d,
                                    const std::vector<double> &x,
                                    std::vector<double> &cnt) {
  int   i, j,  nn = n;
  int ii, jj, iij;
  double xmin, xmax, rang, dd;
  for (i = 0; i < nb; i++)
    cnt.push_back(0);
  xmin = xmax = x[0];
  for (i = 1; i < nn; i++) {
    xmin = std::min(xmin, x[i]);
    xmax = std::min(xmax, x[i]);
  }
  rang = (xmax - xmin) * 1.01;
  *d = dd = rang / (nb);
  for (i = 1; i < nn; i++) {
    ii = (int)(x[i] / dd);
    for (j = 0; j < i; j++) {
      jj = (int)(x[j] / dd);
      iij = abs9((ii - jj));
      cnt[iij]++;
    }
  }
}

void DiscreteIIDModel::band_phi6_bin(int n,
                                       int nb,
                                       double d,
                                       std::vector<double> &x,
                                       double h,
                                       double *u) {
  int   i, nn = n, nbin = nb;
  double delta, sum, term;
  sum = 0.0;
  for (i = 0; i < nbin; i++) {
    delta = i * (d) / (h);
    delta *= delta;
    if (delta >= 1000) break;
    term = exp(-delta / 2) *
      (delta * delta * delta - 15 * delta * delta + 45 * delta - 15);
    sum += term * x[i];
  }
  sum = 2 * sum - 15 * nn;    /* add in diagonal */
  *u = sum / (nn * (nn - 1) * pow(h, 7.0) * sqrt(2 * M_PI));
}

void DiscreteIIDModel::band_phi4_bin(int n,
                                     int nb,
                                     double d,
                                     std::vector<double> x,
                                     double h,
                                     double *u) {
  int   i, nn = n, nbin = nb;
  double delta, sum, term;

  sum = 0.0;
  for (i = 0; i < nbin; i++) {
    delta = i * (d) / (h);
    delta *= delta;
    if (delta >= 1000) break;
    term = exp(-delta / 2) * (delta * delta - 6 * delta + 3);
    sum += term * x[i];
  }
  sum = 2 * sum + nn * 3;     /* add in diagonal */
  *u = sum / (nn * (nn - 1) * pow(h, 5.0) * sqrt(2 * M_PI));
}

double DiscreteIIDModel::mean(const std::vector<double> &data) {
  double sum = 0.0;
  for (unsigned int i = 0; i < data.size(); i++) {
    sum += data[i];
  }
  return sum/static_cast<double>(data.size());
}

double DiscreteIIDModel::var(const std::vector<double> &data) {
  double data_mean = mean(data);
  double sum = 0.0;
  for (unsigned int i = 0; i < data.size(); i++) {
    sum += (data[i] - data_mean)*(data[i] - data_mean);
  }
  return sum/(static_cast<double>(data.size()) -1.0);
}

double DiscreteIIDModel::quantile(std::vector<double> data, double q) {
  int low_index = static_cast<int>(
    floor(q * (static_cast<double>(data.size()-1))));
  int high_index = static_cast<int>(
    ceil(q * (static_cast<double>(data.size()-1))));
  double h =  q * (static_cast<double>(
    data.size() -1)) - static_cast<double>(low_index);
  sort(data.begin(), data.end());
  return (1-h)*data[low_index] + h * data[high_index];
}

double DiscreteIIDModel::iqr(const std::vector<double> &data) {
  double q1 = quantile(data, 0.25);
  double q2 = quantile(data, 0.75);
  return q2 - q1;
}

double DiscreteIIDModel::kernel_density_estimation(
    double x,
    double bw,
    const std::vector<double> &data) {
  double result = 0.0;
  for (unsigned int i = 0; i < data.size(); i++) {
    double x_xi = static_cast<double>(x) - static_cast<double>(data[i]);
    result += epanechnikov(x_xi, bw);
  }
  result /= data.size();
  return result;
}

double DiscreteIIDModel::sj_bandwidth(const std::vector<double> &data) {
  double n = static_cast<double>(data.size());
  int nb = 1000;
  double d = 1.0;
  double variance =  var(data);
  std::vector<double> count;
  band_den_bin(static_cast<int>(n), nb, &d, data, count);
  double scale = std::min(std::sqrt(variance), iqr(data)/1.349);
  double b = 1.23 * scale * pow(n, (-1.0/9.0));
  double c1 = 1.0 / (2.0*sqrt(M_PI) * n);
  double td;
  band_phi6_bin(static_cast<int>(n),
                static_cast<int>(count.size()),
                d,
                count,
                b,
                &td);
  td = -td;
  if (td < 0 || td != td) {
    //    cerr << "sj_bandwidth (WARNING). Dataset very sparse !!!\n" << endl;
    return 0.001;
  }
  double sdh;
  band_phi4_bin(static_cast<int>(n),
                static_cast<int>(count.size()),
                d,
                count,
                pow(2.394/(n*td), (1.0/7.0)),
                &sdh);
  return pow((c1/sdh), 1.0/5.0);
}

int DiscreteIIDModel::alphabetSize() const {
  return _probabilities.size();
}

double DiscreteIIDModel::probabilityOf(Symbol s) const {
  if (s > _probabilities.size())
    return -HUGE;
  return _probabilities[s];
}

double DiscreteIIDModel::evaluatePosition(const Sequence &s,
                                          unsigned int i) const {
  return probabilityOf(s[i]);
}

Symbol DiscreteIIDModel::choose() const {
  double random = generateRandomDouble();
  for (unsigned int symbol = 0; symbol < _probabilities.size(); symbol++) {
    random -= exp(_probabilities[symbol]);
    if (random <= 0)
      return symbol;
  }
  return _probabilities.size()-1;
}

Symbol DiscreteIIDModel::choosePosition(const Sequence &s,
                                        unsigned int i) const {
  return choose();
}

std::vector<double> DiscreteIIDModel::probabilities() {
  return _probabilities;
}


}  // namespace model
}  // namespace tops
