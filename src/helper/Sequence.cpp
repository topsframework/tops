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

// ToPS headers
#include "helper/Random.hpp"

// Interface header
#include "helper/Sequence.hpp"

namespace tops {
namespace helper {

/*----------------------------------------------------------------------------*/
/*                              LOCAL FUNCTIONS                               */
/*----------------------------------------------------------------------------*/

static void generateAllCombinationsOfSymbolsImpl(
    unsigned int pos,
    model::Sequence s,
    std::vector<model::Sequence>& sequences) {
  model::Sequence s0 = s;
  s0[pos] = 0;

  model::Sequence s1 = s;
  s1[pos] = 1;

  if (pos < s.size() - 1) {
    generateAllCombinationsOfSymbolsImpl(pos + 1, s0, sequences);
    generateAllCombinationsOfSymbolsImpl(pos + 1, s1, sequences);
  } else {
    sequences.push_back(s0);
    sequences.push_back(s1);
  }
}

/*----------------------------------------------------------------------------*/
/*                                 FUNCTIONS                                  */
/*----------------------------------------------------------------------------*/

model::Sequence generateRandomSequence(
    unsigned int size, unsigned int alphabet_size) {
  model::Sequence sequence;
  for (unsigned int i = 0; i < size; i++) {
    sequence.push_back(generateRandomInteger(alphabet_size-1));
  }
  return sequence;
}

/*----------------------------------------------------------------------------*/

model::Sequence sequenceOfLengths() {
  return { 37, 4890, 1665, 81, 4083, 38, 198, 124, 3231, 1317, 96, 7137, 3162,
           51, 3168, 978, 378, 2787, 196, 2142, 3030, 477, 651, 2334, 918,
           55, 7440, 72, 46, 73, 879, 18, 6051, 1722, 2592, 1011, 539,
           76, 989, 1521, 1209, 2901, 448, 1893, 25, 1845, 165, 3951, 1396,
           82, 1353, 165, 891, 553, 1992, 1014, 3750, 789, 3131, 4, 1536,
           654, 2765, 927, 9, 366, 7368, 6342, 1589, 9508, 1122, 122, 3717,
           1824, 1952, 3270, 300, 1590, 73, 1446, 372, 771, 1560, 966, 1293,
           936, 246, 339, 189, 139, 5414, 2275, 933, 66, 1197, 1908, 6714,
           76, 435, 12414, 4320, 1745, 775, 79, 3, 4338, 2823, 69, 4326,
           28, 834, 1383, 1665, 4317, 281, 1683, 3477, 688, 3699, 2667, 1737,
           7029, 6189, 84, 3912, 1347, 2970, 1081, 2449, 2046, 459, 103, 516,
           75, 543, 2112, 2028, 5358, 31, 951, 54, 97, 194, 567, 3537,
           807, 201, 1156, 5142, 82, 96, 78, 1449, 1737, 1170, 2700, 1098,
           4860, 492, 1134, 3507, 5406, 1314, 912, 2727, 1695, 1, 391, 5433,
           1503, 1668, 651, 1683, 3737, 582, 201, 199, 1065, 243, 6663, 4312,
           31, 1692, 3576, 1629, 1395, 906, 3108, 4008, 1479, 2648, 3015, 636,
           762, 110, 1371, 1200, 273, 4152, 9402, 5076, 3708, 732, 169, 2883,
           408, 3123, 577, 4326, 233, 786, 1932, 1674, 58, 3255, 291, 18,
           36, 562, 2154, 1311, 69, 1725, 576, 1086, 1839, 2028, 154, 2022,
           20307, 1704, 699, 13, 1137, 3657, 608, 540, 3003, 1740, 585, 1938,
           4899, 408, 27, 2880, 2046, 2190, 2319, 3018, 2433, 1311, 1510, 1965,
           843, 44, 5065, 1017, 3, 52, 4233, 3816, 642, 4710, 85, 6553,
           66, 3966, 108, 363, 102, 237, 3573, 214, 2643, 3379, 797, 5466,
           2360, 1833, 960, 5754, 136, 55, 746, 263, 238, 2106, 2499, 196,
           2007, 1177, 1302, 34, 67, 612, 1623, 324, 32, 1167, 1680, 175,
           1395, 203, 7500, 2772, 5991, 1284, 1860, 61, 351, 2079, 6678, 1977,
           19, 8736, 48, 5991, 51, 4186, 464, 657, 108, 1431, 1644, 1920,
           4, 102, 19, 1284, 72, 83, 8289, 72, 1116, 1839, 2241, 7524,
           1824, 220, 1830, 1389, 2883, 475, 429, 18231, 33, 2946, 846, 5577,
           7380, 210, 1833, 525, 3626, 2613, 3492, 135, 3815, 62, 14, 175,
           615, 214, 2478, 51, 8622, 375, 878, 69, 573, 5514, 5958, 1100,
           130, 963, 108, 201, 3477, 3525, 6399, 2609, 1695, 1824, 789, 1041,
           201, 69, 214, 1248, 9216, 145, 2067, 3075, 2295, 22, 984, 96,
           1494, 55, 1173, 1530, 2058, 744, 3417, 1386, 1029, 253, 1890, 117,
           3417, 9492, 775, 318, 843, 58, 80, 1311, 1560, 1263, 36, 237,
           975, 1768, 606, 966, 2091, 148, 822, 2016, 825, 121, 2625, 101,
           3731, 155, 726, 2979, 229, 208, 77, 265, 4896, 3510, 612, 1383,
           1833, 2938, 300, 175, 5373, 303, 195, 29, 2901, 3219, 1287, 106,
           2043, 143, 1413, 1191, 4021, 208, 2345, 1287, 5868, 1860, 432, 1680,
           54, 1524, 657, 793, 735, 6848, 140, 9, 50, 1968, 1128, 2796,
           348, 54, 1548, 1443, 1749, 796, 86, 9207, 2532, 5520, 4194, 106,
           7731, 301, 238, 3960, 4014, 398, 2931, 4680, 101, 167, 181, 52,
           14, 636, 3546, 1143, 46, 56, 894, 1596, 8154, 739, 384, 913,
           451, 2127, 40, 2304, 180, 5124, 981, 94, 24, 339, 585, 1335,
           2952, 303, 3015, 2943, 1938, 225, 1011, 2937, 54, 1605, 1044, 30183,
           6558, 1905, 2865, 3384, 87, 2619, 68, 1054, 289, 54, 9, 9003,
           5739, 2052, 2205, 810, 184, 2382, 16, 4119, 4470, 310, 6441, 69,
           192, 298, 1248, 69, 627, 564, 78, 2712, 2127, 3393, 1917, 1635,
           792, 324, 2741, 2697, 319, 456, 5382, 61, 4860, 1362, 3759, 5631,
           4011, 2757, 1010, 9441, 452, 125, 2292, 6033, 31, 505, 702, 1767,
           3312, 2763, 69, 4215, 2832, 81, 3510, 2517, 70, 612, 429, 75,
           213, 167, 2136, 16, 7689, 1518, 2253, 150, 2283, 120, 1397, 153,
           2421, 4386, 1827, 1830, 7422, 391, 1431, 174, 60, 1974, 4275, 51,
           100, 4581, 1488, 1926, 1377, 4224, 353, 3816, 3921, 55, 37, 198,
           2022, 46, 1275, 1982, 4268, 165, 828, 1839, 632, 61, 4302, 1023,
           9633, 1551, 167, 2967, 167, 1863, 3567, 706, 4104, 130, 2181, 43,
           445, 2718, 2460, 813, 147, 229, 1023, 1555, 1167, 1725, 540, 2259,
           2604, 417, 100, 91, 2964, 573, 1329, 5160, 1128, 1476, 2184, 1563,
           1143, 2818, 152, 5, 8922, 16, 2016, 96, 6765, 678, 228, 1359,
           1197, 639, 10419, 1989, 67, 282, 694, 1911, 396, 1164, 62, 4437,
           818, 642, 219, 999, 855, 5253, 912, 30, 108, 122, 4083, 922,
           4626, 78, 1179, 93, 4062, 2470, 765, 9, 362, 3744, 193, 534,
           243, 153, 2114, 685, 81, 5004, 3888, 2129, 5136, 8460, 978, 38,
           3666, 7674, 1225, 17, 1950, 8857, 1317, 2865, 597, 217, 2472, 1100,
           732, 2917, 1272, 7740, 3190, 978, 124, 2262, 62, 79, 5355, 1068,
           45, 361, 1356, 609, 1821, 1296, 137, 329, 3015, 1866, 312, 504,
           6, 33, 4841, 166, 240, 87, 33, 108, 952, 204, 5478, 183,
           2553, 72, 16, 25, 4228, 105, 119, 117, 2155, 687, 255, 51,
           187, 127, 96, 210, 52, 2370, 207, 5985, 2114, 182, 1349, 3583,
           234, 1581, 105, 236, 559, 98, 93, 57, 213, 183, 77, 9,
           3024, 149, 3850, 56, 4135, 33, 103, 54, 57, 34, 24, 130,
           141, 524, 204, 167, 50, 7710, 2097, 64, 324, 1106, 1194, 69,
           126, 117, 56, 217, 70, 962, 54, 25, 198, 1306, 4032, 22,
           73, 60, 189, 111, 3490, 85, 573, 5607, 273, 523, 126, 8150,
           189, 38, 1104, 12, 27, 189, 261, 124, 45, 1616, 156, 5343,
           223, 3271, 356, 57, 475, 134, 1274, 249, 2733, 4947, 7, 16,
           94, 12, 201, 69, 90, 46, 5, 3630, 130, 122, 114, 4147,
           1038, 274, 69, 298, 213, 84, 90, 1111, 1667, 133, 123, 2280,
           880, 69, 201, 119, 157, 33, 78, 3950, 53, 83, 67, 20,
           20, 142, 1676, 56, 2489, 399, 93, 1106, 66, 59, 1381, 2718,
           113, 187, 480, 34, 168, 118, 2915, 1489, 5253, 1363, 3058, 66,
           17, 48, 440, 484, 122, 232, 316, 33, 126, 270, 64, 402,
           96, 90, 78, 143, 260, 5292, 248, 103, 75, 198, 320, 1615,
           2280, 430, 188, 52, 375, 126, 108, 214, 118, 121, 40, 3,
           10, 87, 84, 320, 147, 189, 956, 1061, 18, 354, 12, 137,
           18, 78, 6169, 163, 4, 142, 163, 78, 378, 62, 399, 273,
           146, 831, 1084, 147, 73, 34, 310, 326, 500, 140, 1467, 143,
           54, 1284, 419, 103, 123, 203, 17, 132, 5226, 531, 1318, 131,
           54, 783, 57, 137, 667, 102, 2394, 34, 6157, 425, 82, 5559,
           649, 2608, 69, 1645, 471, 10503, 34, 193, 2061, 1093, 7398, 75,
           122, 77, 141, 13281, 96, 420, 68, 2329, 384, 8841, 906, 789,
           148, 916, 1422, 2084, 1374, 1141, 6000, 3395, 3091, 4774, 342, 238,
           28, 55, 288, 91, 2842, 189, 167, 88, 108, 2094, 304, 3940,
           129, 150, 137, 2742, 3, 585, 100, 189, 5283, 52, 146, 536,
           109, 699, 24, 687, 833, 112, 6195, 75, 69, 108, 110, 170,
           69, 270, 579, 1240, 90, 55, 163, 2241, 448, 177, 98, 80,
           92, 129, 57, 7152, 396, 38, 3594, 94, 103, 1206, 55, 37,
           69, 8, 150, 88, 9331, 161, 3000, 1363, 67, 79, 114, 126,
           150, 3201, 73, 184, 3760, 11264, 6413, 2681, 2303, 114, 232, 589,
           2439, 285, 395, 3, 3, 83, 92, 43, 15, 1595, 8844, 48,
           100, 226, 104, 246, 5250 };
}

/*----------------------------------------------------------------------------*/

std::vector<model::Sequence>
generateAllCombinationsOfSymbols(unsigned int size) {
  model::Sequence s(size, 0);
  std::vector<model::Sequence> sequences;
  generateAllCombinationsOfSymbolsImpl(0, s, sequences);
  return sequences;
}

/*----------------------------------------------------------------------------*/

}  // namespace helper
}  // namespace tops
