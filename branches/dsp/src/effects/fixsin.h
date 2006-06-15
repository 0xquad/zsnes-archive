/*
Copyright (C) 1997-2006 ZSNES Team ( zsKnight, _Demo_, pagefault, Nach )

http://www.zsnes.com
http://sourceforge.net/projects/zsnes

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#define FSINMAX 2047
#define FSin(x) FSinTab[(x)&FSINMAX]
#define FCos(x) FCosTab[(x)&FSINMAX]

int FSinTab[2048] =
{
0, 201, 402, 603, 804, 1005, 1206, 1407, 1608, 1809, 2010, 2211, 2412, 2613,
2814, 3014, 3215, 3416, 3617, 3818, 4018, 4219, 4420, 4620, 4821, 5021, 5222,
5422, 5622, 5823, 6023, 6223, 6423, 6623, 6823, 7023, 7223, 7423, 7623, 7822,
8022, 8221, 8421, 8620, 8819, 9019, 9218, 9417, 9616, 9814, 10013, 10212, 10410,
10609, 10807, 11006, 11204, 11402, 11600, 11797, 11995, 12193, 12390, 12588,
12785, 12982, 13179, 13376, 13573, 13769, 13966, 14162, 14359, 14555, 14751,
14946, 15142, 15338, 15533, 15728, 15923, 16118, 16313, 16508, 16702, 16897,
17091, 17285, 17479, 17672, 17866, 18059, 18253, 18446, 18638, 18831, 19024,
19216, 19408, 19600, 19792, 19983, 20175, 20366, 20557, 20748, 20938, 21129,
21319, 21509, 21699, 21889, 22078, 22267, 22456, 22645, 22833, 23022, 23210,
23398, 23586, 23773, 23960, 24147, 24334, 24521, 24707, 24893, 25079, 25265,
25450, 25635, 25820, 26005, 26189, 26373, 26557, 26741, 26925, 27108, 27291,
27473, 27656, 27838, 28020, 28201, 28383, 28564, 28745, 28925, 29105, 29285,
29465, 29645, 29824, 30003, 30181, 30360, 30538, 30715, 30893, 31070, 31247,
31424, 31600, 31776, 31952, 32127, 32302, 32477, 32651, 32826, 32999, 33173,
33346, 33519, 33692, 33864, 34036, 34208, 34379, 34550, 34721, 34891, 35061,
35231, 35400, 35569, 35738, 35906, 36074, 36242, 36409, 36576, 36743, 36909,
37075, 37241, 37406, 37571, 37736, 37900, 38064, 38227, 38390, 38553, 38716,
38878, 39039, 39201, 39362, 39522, 39682, 39842, 40002, 40161, 40319, 40478,
40636, 40793, 40950, 41107, 41263, 41419, 41575, 41730, 41885, 42040, 42194,
42347, 42501, 42653, 42806, 42958, 43110, 43261, 43412, 43562, 43712, 43862,
44011, 44160, 44308, 44456, 44603, 44750, 44897, 45043, 45189, 45335, 45480,
45624, 45768, 45912, 46055, 46198, 46340, 46482, 46624, 46765, 46906, 47046,
47186, 47325, 47464, 47602, 47740, 47878, 48015, 48151, 48288, 48423, 48558,
48693, 48828, 48961, 49095, 49228, 49360, 49492, 49624, 49755, 49886, 50016,
50146, 50275, 50403, 50532, 50660, 50787, 50914, 51040, 51166, 51291, 51416,
51541, 51665, 51788, 51911, 52033, 52155, 52277, 52398, 52518, 52639, 52758,
52877, 52996, 53114, 53231, 53348, 53465, 53581, 53696, 53811, 53926, 54040,
54153, 54266, 54379, 54491, 54602, 54713, 54823, 54933, 55043, 55152, 55260,
55368, 55475, 55582, 55688, 55794, 55899, 56004, 56108, 56212, 56315, 56417,
56519, 56621, 56722, 56822, 56922, 57022, 57120, 57219, 57316, 57414, 57510,
57606, 57702, 57797, 57892, 57986, 58079, 58172, 58264, 58356, 58447, 58538,
58628, 58718, 58807, 58895, 58983, 59070, 59157, 59243, 59329, 59414, 59499,
59583, 59666, 59749, 59831, 59913, 59994, 60075, 60155, 60235, 60313, 60392,
60470, 60547, 60624, 60700, 60775, 60850, 60924, 60998, 61071, 61144, 61216,
61288, 61359, 61429, 61499, 61568, 61637, 61705, 61772, 61839, 61905, 61971,
62036, 62100, 62164, 62228, 62291, 62353, 62414, 62475, 62536, 62596, 62655,
62714, 62772, 62829, 62886, 62942, 62998, 63053, 63108, 63162, 63215, 63268,
63320, 63371, 63422, 63473, 63522, 63571, 63620, 63668, 63715, 63762, 63808,
63854, 63899, 63943, 63987, 64030, 64073, 64115, 64156, 64197, 64237, 64276,
64315, 64353, 64391, 64428, 64465, 64501, 64536, 64571, 64605, 64638, 64671,
64703, 64735, 64766, 64796, 64826, 64855, 64884, 64912, 64939, 64966, 64992,
65018, 65043, 65067, 65091, 65114, 65136, 65158, 65179, 65200, 65220, 65239,
65258, 65276, 65294, 65311, 65327, 65343, 65358, 65372, 65386, 65400, 65412,
65424, 65436, 65446, 65457, 65466, 65475, 65483, 65491, 65498, 65505, 65511,
65516, 65520, 65524, 65528, 65531, 65533, 65534, 65535, 65536, 65535, 65534,
65533, 65531, 65528, 65524, 65520, 65516, 65511, 65505, 65498, 65491, 65483,
65475, 65466, 65457, 65446, 65436, 65424, 65412, 65400, 65386, 65372, 65358,
65343, 65327, 65311, 65294, 65276, 65258, 65239, 65220, 65200, 65179, 65158,
65136, 65114, 65091, 65067, 65043, 65018, 64992, 64966, 64939, 64912, 64884,
64855, 64826, 64796, 64766, 64735, 64703, 64671, 64638, 64605, 64571, 64536,
64501, 64465, 64428, 64391, 64353, 64315, 64276, 64237, 64197, 64156, 64115,
64073, 64030, 63987, 63943, 63899, 63854, 63808, 63762, 63715, 63668, 63620,
63571, 63522, 63473, 63422, 63371, 63320, 63268, 63215, 63162, 63108, 63053,
62998, 62942, 62886, 62829, 62772, 62714, 62655, 62596, 62536, 62475, 62414,
62353, 62291, 62228, 62164, 62100, 62036, 61971, 61905, 61839, 61772, 61705,
61637, 61568, 61499, 61429, 61359, 61288, 61216, 61144, 61071, 60998, 60924,
60850, 60775, 60700, 60624, 60547, 60470, 60392, 60313, 60235, 60155, 60075,
59994, 59913, 59831, 59749, 59666, 59583, 59499, 59414, 59329, 59243, 59157,
59070, 58983, 58895, 58807, 58718, 58628, 58538, 58447, 58356, 58264, 58172,
58079, 57986, 57892, 57797, 57702, 57606, 57510, 57414, 57316, 57219, 57120,
57022, 56922, 56822, 56722, 56621, 56519, 56417, 56315, 56212, 56108, 56004,
55899, 55794, 55688, 55582, 55475, 55368, 55260, 55152, 55043, 54933, 54823,
54713, 54602, 54491, 54379, 54266, 54153, 54040, 53926, 53811, 53696, 53581,
53465, 53348, 53231, 53114, 52996, 52877, 52758, 52639, 52518, 52398, 52277,
52155, 52033, 51911, 51788, 51665, 51541, 51416, 51291, 51166, 51040, 50914,
50787, 50660, 50532, 50403, 50275, 50146, 50016, 49886, 49755, 49624, 49492,
49360, 49228, 49095, 48961, 48828, 48693, 48558, 48423, 48288, 48151, 48015,
47878, 47740, 47602, 47464, 47325, 47186, 47046, 46906, 46765, 46624, 46482,
46340, 46198, 46055, 45912, 45768, 45624, 45480, 45335, 45189, 45043, 44897,
44750, 44603, 44456, 44308, 44160, 44011, 43862, 43712, 43562, 43412, 43261,
43110, 42958, 42806, 42653, 42501, 42347, 42194, 42040, 41885, 41730, 41575,
41419, 41263, 41107, 40950, 40793, 40636, 40478, 40319, 40161, 40002, 39842,
39682, 39522, 39362, 39201, 39039, 38878, 38716, 38553, 38390, 38227, 38064,
37900, 37736, 37571, 37406, 37241, 37075, 36909, 36743, 36576, 36409, 36242,
36074, 35906, 35738, 35569, 35400, 35231, 35061, 34891, 34721, 34550, 34379,
34208, 34036, 33864, 33692, 33519, 33346, 33173, 32999, 32826, 32651, 32477,
32302, 32127, 31952, 31776, 31600, 31424, 31247, 31070, 30893, 30715, 30538,
30360, 30181, 30003, 29824, 29645, 29465, 29285, 29105, 28925, 28745, 28564,
28383, 28201, 28020, 27838, 27656, 27473, 27291, 27108, 26925, 26741, 26557,
26373, 26189, 26005, 25820, 25635, 25450, 25265, 25079, 24893, 24707, 24521,
24334, 24147, 23960, 23773, 23586, 23398, 23210, 23022, 22833, 22645, 22456,
22267, 22078, 21889, 21699, 21509, 21319, 21129, 20938, 20748, 20557, 20366,
20175, 19983, 19792, 19600, 19408, 19216, 19024, 18831, 18638, 18446, 18253,
18059, 17866, 17672, 17479, 17285, 17091, 16897, 16702, 16508, 16313, 16118,
15923, 15728, 15533, 15338, 15142, 14946, 14751, 14555, 14359, 14162, 13966,
13769, 13573, 13376, 13179, 12982, 12785, 12588, 12390, 12193, 11995, 11797,
11600, 11402, 11204, 11006, 10807, 10609, 10410, 10212, 10013, 9814, 9616, 9417,
9218, 9019, 8819, 8620, 8421, 8221, 8022, 7822, 7623, 7423, 7223, 7023, 6823,
6623, 6423, 6223, 6023, 5823, 5622, 5422, 5222, 5021, 4821, 4620, 4420, 4219,
4018, 3818, 3617, 3416, 3215, 3014, 2814, 2613, 2412, 2211, 2010, 1809, 1608,
1407, 1206, 1005, 804, 603, 402, 201, 0, -201, -402, -603, -804, -1005, -1206,
-1407, -1608, -1809, -2010, -2211, -2412, -2613, -2814, -3014, -3215, -3416,
-3617, -3818, -4018, -4219, -4420, -4620, -4821, -5021, -5222, -5422, -5622,
-5823, -6023, -6223, -6423, -6623, -6823, -7023, -7223, -7423, -7623, -7822,
-8022, -8221, -8421, -8620, -8819, -9019, -9218, -9417, -9616, -9814, -10013,
-10212, -10410, -10609, -10807, -11006, -11204, -11402, -11600, -11797, -11995,
-12193, -12390, -12588, -12785, -12982, -13179, -13376, -13573, -13769, -13966,
-14162, -14359, -14555, -14751, -14946, -15142, -15338, -15533, -15728, -15923,
-16118, -16313, -16508, -16702, -16897, -17091, -17285, -17479, -17672, -17866,
-18059, -18253, -18446, -18638, -18831, -19024, -19216, -19408, -19600, -19792,
-19983, -20175, -20366, -20557, -20748, -20938, -21129, -21319, -21509, -21699,
-21889, -22078, -22267, -22456, -22645, -22833, -23022, -23210, -23398, -23586,
-23773, -23960, -24147, -24334, -24521, -24707, -24893, -25079, -25265, -25450,
-25635, -25820, -26005, -26189, -26373, -26557, -26741, -26925, -27108, -27291,
-27473, -27656, -27838, -28020, -28201, -28383, -28564, -28745, -28925, -29105,
-29285, -29465, -29645, -29824, -30003, -30181, -30360, -30538, -30715, -30893,
-31070, -31247, -31424, -31600, -31776, -31952, -32127, -32302, -32477, -32651,
-32826, -32999, -33173, -33346, -33519, -33692, -33864, -34036, -34208, -34379,
-34550, -34721, -34891, -35061, -35231, -35400, -35569, -35738, -35906, -36074,
-36242, -36409, -36576, -36743, -36909, -37075, -37241, -37406, -37571, -37736,
-37900, -38064, -38227, -38390, -38553, -38716, -38878, -39039, -39201, -39362,
-39522, -39682, -39842, -40002, -40161, -40319, -40478, -40636, -40793, -40950,
-41107, -41263, -41419, -41575, -41730, -41885, -42040, -42194, -42347, -42501,
-42653, -42806, -42958, -43110, -43261, -43412, -43562, -43712, -43862, -44011,
-44160, -44308, -44456, -44603, -44750, -44897, -45043, -45189, -45335, -45480,
-45624, -45768, -45912, -46055, -46198, -46340, -46482, -46624, -46765, -46906,
-47046, -47186, -47325, -47464, -47602, -47740, -47878, -48015, -48151, -48288,
-48423, -48558, -48693, -48828, -48961, -49095, -49228, -49360, -49492, -49624,
-49755, -49886, -50016, -50146, -50275, -50403, -50532, -50660, -50787, -50914,
-51040, -51166, -51291, -51416, -51541, -51665, -51788, -51911, -52033, -52155,
-52277, -52398, -52518, -52639, -52758, -52877, -52996, -53114, -53231, -53348,
-53465, -53581, -53696, -53811, -53926, -54040, -54153, -54266, -54379, -54491,
-54602, -54713, -54823, -54933, -55043, -55152, -55260, -55368, -55475, -55582,
-55688, -55794, -55899, -56004, -56108, -56212, -56315, -56417, -56519, -56621,
-56722, -56822, -56922, -57022, -57120, -57219, -57316, -57414, -57510, -57606,
-57702, -57797, -57892, -57986, -58079, -58172, -58264, -58356, -58447, -58538,
-58628, -58718, -58807, -58895, -58983, -59070, -59157, -59243, -59329, -59414,
-59499, -59583, -59666, -59749, -59831, -59913, -59994, -60075, -60155, -60235,
-60313, -60392, -60470, -60547, -60624, -60700, -60775, -60850, -60924, -60998,
-61071, -61144, -61216, -61288, -61359, -61429, -61499, -61568, -61637, -61705,
-61772, -61839, -61905, -61971, -62036, -62100, -62164, -62228, -62291, -62353,
-62414, -62475, -62536, -62596, -62655, -62714, -62772, -62829, -62886, -62942,
-62998, -63053, -63108, -63162, -63215, -63268, -63320, -63371, -63422, -63473,
-63522, -63571, -63620, -63668, -63715, -63762, -63808, -63854, -63899, -63943,
-63987, -64030, -64073, -64115, -64156, -64197, -64237, -64276, -64315, -64353,
-64391, -64428, -64465, -64501, -64536, -64571, -64605, -64638, -64671, -64703,
-64735, -64766, -64796, -64826, -64855, -64884, -64912, -64939, -64966, -64992,
-65018, -65043, -65067, -65091, -65114, -65136, -65158, -65179, -65200, -65220,
-65239, -65258, -65276, -65294, -65311, -65327, -65343, -65358, -65372, -65386,
-65400, -65412, -65424, -65436, -65446, -65457, -65466, -65475, -65483, -65491,
-65498, -65505, -65511, -65516, -65520, -65524, -65528, -65531, -65533, -65534,
-65535, -65536, -65535, -65534, -65533, -65531, -65528, -65524, -65520, -65516,
-65511, -65505, -65498, -65491, -65483, -65475, -65466, -65457, -65446, -65436,
-65424, -65412, -65400, -65386, -65372, -65358, -65343, -65327, -65311, -65294,
-65276, -65258, -65239, -65220, -65200, -65179, -65158, -65136, -65114, -65091,
-65067, -65043, -65018, -64992, -64966, -64939, -64912, -64884, -64855, -64826,
-64796, -64766, -64735, -64703, -64671, -64638, -64605, -64571, -64536, -64501,
-64465, -64428, -64391, -64353, -64315, -64276, -64237, -64197, -64156, -64115,
-64073, -64030, -63987, -63943, -63899, -63854, -63808, -63762, -63715, -63668,
-63620, -63571, -63522, -63473, -63422, -63371, -63320, -63268, -63215, -63162,
-63108, -63053, -62998, -62942, -62886, -62829, -62772, -62714, -62655, -62596,
-62536, -62475, -62414, -62353, -62291, -62228, -62164, -62100, -62036, -61971,
-61905, -61839, -61772, -61705, -61637, -61568, -61499, -61429, -61359, -61288,
-61216, -61144, -61071, -60998, -60924, -60850, -60775, -60700, -60624, -60547,
-60470, -60392, -60313, -60235, -60155, -60075, -59994, -59913, -59831, -59749,
-59666, -59583, -59499, -59414, -59329, -59243, -59157, -59070, -58983, -58895,
-58807, -58718, -58628, -58538, -58447, -58356, -58264, -58172, -58079, -57986,
-57892, -57797, -57702, -57606, -57510, -57414, -57316, -57219, -57120, -57022,
-56922, -56822, -56722, -56621, -56519, -56417, -56315, -56212, -56108, -56004,
-55899, -55794, -55688, -55582, -55475, -55368, -55260, -55152, -55043, -54933,
-54823, -54713, -54602, -54491, -54379, -54266, -54153, -54040, -53926, -53811,
-53696, -53581, -53465, -53348, -53231, -53114, -52996, -52877, -52758, -52639,
-52518, -52398, -52277, -52155, -52033, -51911, -51788, -51665, -51541, -51416,
-51291, -51166, -51040, -50914, -50787, -50660, -50532, -50403, -50275, -50146,
-50016, -49886, -49755, -49624, -49492, -49360, -49228, -49095, -48961, -48828,
-48693, -48558, -48423, -48288, -48151, -48015, -47878, -47740, -47602, -47464,
-47325, -47186, -47046, -46906, -46765, -46624, -46482, -46340, -46198, -46055,
-45912, -45768, -45624, -45480, -45335, -45189, -45043, -44897, -44750, -44603,
-44456, -44308, -44160, -44011, -43862, -43712, -43562, -43412, -43261, -43110,
-42958, -42806, -42653, -42501, -42347, -42194, -42040, -41885, -41730, -41575,
-41419, -41263, -41107, -40950, -40793, -40636, -40478, -40319, -40161, -40002,
-39842, -39682, -39522, -39362, -39201, -39039, -38878, -38716, -38553, -38390,
-38227, -38064, -37900, -37736, -37571, -37406, -37241, -37075, -36909, -36743,
-36576, -36409, -36242, -36074, -35906, -35738, -35569, -35400, -35231, -35061,
-34891, -34721, -34550, -34379, -34208, -34036, -33864, -33692, -33519, -33346,
-33173, -32999, -32826, -32651, -32477, -32302, -32127, -31952, -31776, -31600,
-31424, -31247, -31070, -30893, -30715, -30538, -30360, -30181, -30003, -29824,
-29645, -29465, -29285, -29105, -28925, -28745, -28564, -28383, -28201, -28020,
-27838, -27656, -27473, -27291, -27108, -26925, -26741, -26557, -26373, -26189,
-26005, -25820, -25635, -25450, -25265, -25079, -24893, -24707, -24521, -24334,
-24147, -23960, -23773, -23586, -23398, -23210, -23022, -22833, -22645, -22456,
-22267, -22078, -21889, -21699, -21509, -21319, -21129, -20938, -20748, -20557,
-20366, -20175, -19983, -19792, -19600, -19408, -19216, -19024, -18831, -18638,
-18446, -18253, -18059, -17866, -17672, -17479, -17285, -17091, -16897, -16702,
-16508, -16313, -16118, -15923, -15728, -15533, -15338, -15142, -14946, -14751,
-14555, -14359, -14162, -13966, -13769, -13573, -13376, -13179, -12982, -12785,
-12588, -12390, -12193, -11995, -11797, -11600, -11402, -11204, -11006, -10807,
-10609, -10410, -10212, -10013, -9814, -9616, -9417, -9218, -9019, -8819, -8620,
-8421, -8221, -8022, -7822, -7623, -7423, -7223, -7023, -6823, -6623, -6423,
-6223, -6023, -5823, -5622, -5422, -5222, -5021, -4821, -4620, -4420, -4219,
-4018, -3818, -3617, -3416, -3215, -3014, -2814, -2613, -2412, -2211, -2010,
-1809, -1608, -1407, -1206, -1005, -804, -603, -402, -201
};
int FCosTab[2048] =
{
65536, 65535, 65534, 65533, 65531, 65528, 65524, 65520, 65516, 65511, 65505,
65498, 65491, 65483, 65475, 65466, 65457, 65446, 65436, 65424, 65412, 65400,
65386, 65372, 65358, 65343, 65327, 65311, 65294, 65276, 65258, 65239, 65220,
65200, 65179, 65158, 65136, 65114, 65091, 65067, 65043, 65018, 64992, 64966,
64939, 64912, 64884, 64855, 64826, 64796, 64766, 64735, 64703, 64671, 64638,
64605, 64571, 64536, 64501, 64465, 64428, 64391, 64353, 64315, 64276, 64237,
64197, 64156, 64115, 64073, 64030, 63987, 63943, 63899, 63854, 63808, 63762,
63715, 63668, 63620, 63571, 63522, 63473, 63422, 63371, 63320, 63268, 63215,
63162, 63108, 63053, 62998, 62942, 62886, 62829, 62772, 62714, 62655, 62596,
62536, 62475, 62414, 62353, 62291, 62228, 62164, 62100, 62036, 61971, 61905,
61839, 61772, 61705, 61637, 61568, 61499, 61429, 61359, 61288, 61216, 61144,
61071, 60998, 60924, 60850, 60775, 60700, 60624, 60547, 60470, 60392, 60313,
60235, 60155, 60075, 59994, 59913, 59831, 59749, 59666, 59583, 59499, 59414,
59329, 59243, 59157, 59070, 58983, 58895, 58807, 58718, 58628, 58538, 58447,
58356, 58264, 58172, 58079, 57986, 57892, 57797, 57702, 57606, 57510, 57414,
57316, 57219, 57120, 57022, 56922, 56822, 56722, 56621, 56519, 56417, 56315,
56212, 56108, 56004, 55899, 55794, 55688, 55582, 55475, 55368, 55260, 55152,
55043, 54933, 54823, 54713, 54602, 54491, 54379, 54266, 54153, 54040, 53926,
53811, 53696, 53581, 53465, 53348, 53231, 53114, 52996, 52877, 52758, 52639,
52518, 52398, 52277, 52155, 52033, 51911, 51788, 51665, 51541, 51416, 51291,
51166, 51040, 50914, 50787, 50660, 50532, 50403, 50275, 50146, 50016, 49886,
49755, 49624, 49492, 49360, 49228, 49095, 48961, 48828, 48693, 48558, 48423,
48288, 48151, 48015, 47878, 47740, 47602, 47464, 47325, 47186, 47046, 46906,
46765, 46624, 46482, 46340, 46198, 46055, 45912, 45768, 45624, 45480, 45335,
45189, 45043, 44897, 44750, 44603, 44456, 44308, 44160, 44011, 43862, 43712,
43562, 43412, 43261, 43110, 42958, 42806, 42653, 42501, 42347, 42194, 42040,
41885, 41730, 41575, 41419, 41263, 41107, 40950, 40793, 40636, 40478, 40319,
40161, 40002, 39842, 39682, 39522, 39362, 39201, 39039, 38878, 38716, 38553,
38390, 38227, 38064, 37900, 37736, 37571, 37406, 37241, 37075, 36909, 36743,
36576, 36409, 36242, 36074, 35906, 35738, 35569, 35400, 35231, 35061, 34891,
34721, 34550, 34379, 34208, 34036, 33864, 33692, 33519, 33346, 33173, 32999,
32826, 32651, 32477, 32302, 32127, 31952, 31776, 31600, 31424, 31247, 31070,
30893, 30715, 30538, 30360, 30181, 30003, 29824, 29645, 29465, 29285, 29105,
28925, 28745, 28564, 28383, 28201, 28020, 27838, 27656, 27473, 27291, 27108,
26925, 26741, 26557, 26373, 26189, 26005, 25820, 25635, 25450, 25265, 25079,
24893, 24707, 24521, 24334, 24147, 23960, 23773, 23586, 23398, 23210, 23022,
22833, 22645, 22456, 22267, 22078, 21889, 21699, 21509, 21319, 21129, 20938,
20748, 20557, 20366, 20175, 19983, 19792, 19600, 19408, 19216, 19024, 18831,
18638, 18446, 18253, 18059, 17866, 17672, 17479, 17285, 17091, 16897, 16702,
16508, 16313, 16118, 15923, 15728, 15533, 15338, 15142, 14946, 14751, 14555,
14359, 14162, 13966, 13769, 13573, 13376, 13179, 12982, 12785, 12588, 12390,
12193, 11995, 11797, 11600, 11402, 11204, 11006, 10807, 10609, 10410, 10212,
10013, 9814, 9616, 9417, 9218, 9019, 8819, 8620, 8421, 8221, 8022, 7822, 7623,
7423, 7223, 7023, 6823, 6623, 6423, 6223, 6023, 5823, 5622, 5422, 5222, 5021,
4821, 4620, 4420, 4219, 4018, 3818, 3617, 3416, 3215, 3014, 2814, 2613, 2412,
2211, 2010, 1809, 1608, 1407, 1206, 1005, 804, 603, 402, 201, 0, -201, -402,
-603, -804, -1005, -1206, -1407, -1608, -1809, -2010, -2211, -2412, -2613, -2814,
-3014, -3215, -3416, -3617, -3818, -4018, -4219, -4420, -4620, -4821, -5021,
-5222, -5422, -5622, -5823, -6023, -6223, -6423, -6623, -6823, -7023, -7223,
-7423, -7623, -7822, -8022, -8221, -8421, -8620, -8819, -9019, -9218, -9417,
-9616, -9814, -10013, -10212, -10410, -10609, -10807, -11006, -11204, -11402,
-11600, -11797, -11995, -12193, -12390, -12588, -12785, -12982, -13179, -13376,
-13573, -13769, -13966, -14162, -14359, -14555, -14751, -14946, -15142, -15338,
-15533, -15728, -15923, -16118, -16313, -16508, -16702, -16897, -17091, -17285,
-17479, -17672, -17866, -18059, -18253, -18446, -18638, -18831, -19024, -19216,
-19408, -19600, -19792, -19983, -20175, -20366, -20557, -20748, -20938, -21129,
-21319, -21509, -21699, -21889, -22078, -22267, -22456, -22645, -22833, -23022,
-23210, -23398, -23586, -23773, -23960, -24147, -24334, -24521, -24707, -24893,
-25079, -25265, -25450, -25635, -25820, -26005, -26189, -26373, -26557, -26741,
-26925, -27108, -27291, -27473, -27656, -27838, -28020, -28201, -28383, -28564,
-28745, -28925, -29105, -29285, -29465, -29645, -29824, -30003, -30181, -30360,
-30538, -30715, -30893, -31070, -31247, -31424, -31600, -31776, -31952, -32127,
-32302, -32477, -32651, -32826, -32999, -33173, -33346, -33519, -33692, -33864,
-34036, -34208, -34379, -34550, -34721, -34891, -35061, -35231, -35400, -35569,
-35738, -35906, -36074, -36242, -36409, -36576, -36743, -36909, -37075, -37241,
-37406, -37571, -37736, -37900, -38064, -38227, -38390, -38553, -38716, -38878,
-39039, -39201, -39362, -39522, -39682, -39842, -40002, -40161, -40319, -40478,
-40636, -40793, -40950, -41107, -41263, -41419, -41575, -41730, -41885, -42040,
-42194, -42347, -42501, -42653, -42806, -42958, -43110, -43261, -43412, -43562,
-43712, -43862, -44011, -44160, -44308, -44456, -44603, -44750, -44897, -45043,
-45189, -45335, -45480, -45624, -45768, -45912, -46055, -46198, -46340, -46482,
-46624, -46765, -46906, -47046, -47186, -47325, -47464, -47602, -47740, -47878,
-48015, -48151, -48288, -48423, -48558, -48693, -48828, -48961, -49095, -49228,
-49360, -49492, -49624, -49755, -49886, -50016, -50146, -50275, -50403, -50532,
-50660, -50787, -50914, -51040, -51166, -51291, -51416, -51541, -51665, -51788,
-51911, -52033, -52155, -52277, -52398, -52518, -52639, -52758, -52877, -52996,
-53114, -53231, -53348, -53465, -53581, -53696, -53811, -53926, -54040, -54153,
-54266, -54379, -54491, -54602, -54713, -54823, -54933, -55043, -55152, -55260,
-55368, -55475, -55582, -55688, -55794, -55899, -56004, -56108, -56212, -56315,
-56417, -56519, -56621, -56722, -56822, -56922, -57022, -57120, -57219, -57316,
-57414, -57510, -57606, -57702, -57797, -57892, -57986, -58079, -58172, -58264,
-58356, -58447, -58538, -58628, -58718, -58807, -58895, -58983, -59070, -59157,
-59243, -59329, -59414, -59499, -59583, -59666, -59749, -59831, -59913, -59994,
-60075, -60155, -60235, -60313, -60392, -60470, -60547, -60624, -60700, -60775,
-60850, -60924, -60998, -61071, -61144, -61216, -61288, -61359, -61429, -61499,
-61568, -61637, -61705, -61772, -61839, -61905, -61971, -62036, -62100, -62164,
-62228, -62291, -62353, -62414, -62475, -62536, -62596, -62655, -62714, -62772,
-62829, -62886, -62942, -62998, -63053, -63108, -63162, -63215, -63268, -63320,
-63371, -63422, -63473, -63522, -63571, -63620, -63668, -63715, -63762, -63808,
-63854, -63899, -63943, -63987, -64030, -64073, -64115, -64156, -64197, -64237,
-64276, -64315, -64353, -64391, -64428, -64465, -64501, -64536, -64571, -64605,
-64638, -64671, -64703, -64735, -64766, -64796, -64826, -64855, -64884, -64912,
-64939, -64966, -64992, -65018, -65043, -65067, -65091, -65114, -65136, -65158,
-65179, -65200, -65220, -65239, -65258, -65276, -65294, -65311, -65327, -65343,
-65358, -65372, -65386, -65400, -65412, -65424, -65436, -65446, -65457, -65466,
-65475, -65483, -65491, -65498, -65505, -65511, -65516, -65520, -65524, -65528,
-65531, -65533, -65534, -65535, -65536, -65535, -65534, -65533, -65531, -65528,
-65524, -65520, -65516, -65511, -65505, -65498, -65491, -65483, -65475, -65466,
-65457, -65446, -65436, -65424, -65412, -65400, -65386, -65372, -65358, -65343,
-65327, -65311, -65294, -65276, -65258, -65239, -65220, -65200, -65179, -65158,
-65136, -65114, -65091, -65067, -65043, -65018, -64992, -64966, -64939, -64912,
-64884, -64855, -64826, -64796, -64766, -64735, -64703, -64671, -64638, -64605,
-64571, -64536, -64501, -64465, -64428, -64391, -64353, -64315, -64276, -64237,
-64197, -64156, -64115, -64073, -64030, -63987, -63943, -63899, -63854, -63808,
-63762, -63715, -63668, -63620, -63571, -63522, -63473, -63422, -63371, -63320,
-63268, -63215, -63162, -63108, -63053, -62998, -62942, -62886, -62829, -62772,
-62714, -62655, -62596, -62536, -62475, -62414, -62353, -62291, -62228, -62164,
-62100, -62036, -61971, -61905, -61839, -61772, -61705, -61637, -61568, -61499,
-61429, -61359, -61288, -61216, -61144, -61071, -60998, -60924, -60850, -60775,
-60700, -60624, -60547, -60470, -60392, -60313, -60235, -60155, -60075, -59994,
-59913, -59831, -59749, -59666, -59583, -59499, -59414, -59329, -59243, -59157,
-59070, -58983, -58895, -58807, -58718, -58628, -58538, -58447, -58356, -58264,
-58172, -58079, -57986, -57892, -57797, -57702, -57606, -57510, -57414, -57316,
-57219, -57120, -57022, -56922, -56822, -56722, -56621, -56519, -56417, -56315,
-56212, -56108, -56004, -55899, -55794, -55688, -55582, -55475, -55368, -55260,
-55152, -55043, -54933, -54823, -54713, -54602, -54491, -54379, -54266, -54153,
-54040, -53926, -53811, -53696, -53581, -53465, -53348, -53231, -53114, -52996,
-52877, -52758, -52639, -52518, -52398, -52277, -52155, -52033, -51911, -51788,
-51665, -51541, -51416, -51291, -51166, -51040, -50914, -50787, -50660, -50532,
-50403, -50275, -50146, -50016, -49886, -49755, -49624, -49492, -49360, -49228,
-49095, -48961, -48828, -48693, -48558, -48423, -48288, -48151, -48015, -47878,
-47740, -47602, -47464, -47325, -47186, -47046, -46906, -46765, -46624, -46482,
-46340, -46198, -46055, -45912, -45768, -45624, -45480, -45335, -45189, -45043,
-44897, -44750, -44603, -44456, -44308, -44160, -44011, -43862, -43712, -43562,
-43412, -43261, -43110, -42958, -42806, -42653, -42501, -42347, -42194, -42040,
-41885, -41730, -41575, -41419, -41263, -41107, -40950, -40793, -40636, -40478,
-40319, -40161, -40002, -39842, -39682, -39522, -39362, -39201, -39039, -38878,
-38716, -38553, -38390, -38227, -38064, -37900, -37736, -37571, -37406, -37241,
-37075, -36909, -36743, -36576, -36409, -36242, -36074, -35906, -35738, -35569,
-35400, -35231, -35061, -34891, -34721, -34550, -34379, -34208, -34036, -33864,
-33692, -33519, -33346, -33173, -32999, -32826, -32651, -32477, -32302, -32127,
-31952, -31776, -31600, -31424, -31247, -31070, -30893, -30715, -30538, -30360,
-30181, -30003, -29824, -29645, -29465, -29285, -29105, -28925, -28745, -28564,
-28383, -28201, -28020, -27838, -27656, -27473, -27291, -27108, -26925, -26741,
-26557, -26373, -26189, -26005, -25820, -25635, -25450, -25265, -25079, -24893,
-24707, -24521, -24334, -24147, -23960, -23773, -23586, -23398, -23210, -23022,
-22833, -22645, -22456, -22267, -22078, -21889, -21699, -21509, -21319, -21129,
-20938, -20748, -20557, -20366, -20175, -19983, -19792, -19600, -19408, -19216,
-19024, -18831, -18638, -18446, -18253, -18059, -17866, -17672, -17479, -17285,
-17091, -16897, -16702, -16508, -16313, -16118, -15923, -15728, -15533, -15338,
-15142, -14946, -14751, -14555, -14359, -14162, -13966, -13769, -13573, -13376,
-13179, -12982, -12785, -12588, -12390, -12193, -11995, -11797, -11600, -11402,
-11204, -11006, -10807, -10609, -10410, -10212, -10013, -9814, -9616, -9417,
-9218, -9019, -8819, -8620, -8421, -8221, -8022, -7822, -7623, -7423, -7223,
-7023, -6823, -6623, -6423, -6223, -6023, -5823, -5622, -5422, -5222, -5021,
-4821, -4620, -4420, -4219, -4018, -3818, -3617, -3416, -3215, -3014, -2814,
-2613, -2412, -2211, -2010, -1809, -1608, -1407, -1206, -1005, -804, -603, -402,
-201, 0, 201, 402, 603, 804, 1005, 1206, 1407, 1608, 1809, 2010, 2211, 2412,
2613, 2814, 3014, 3215, 3416, 3617, 3818, 4018, 4219, 4420, 4620, 4821, 5021,
5222, 5422, 5622, 5823, 6023, 6223, 6423, 6623, 6823, 7023, 7223, 7423, 7623,
7822, 8022, 8221, 8421, 8620, 8819, 9019, 9218, 9417, 9616, 9814, 10013, 10212,
10410, 10609, 10807, 11006, 11204, 11402, 11600, 11797, 11995, 12193, 12390,
12588, 12785, 12982, 13179, 13376, 13573, 13769, 13966, 14162, 14359, 14555,
14751, 14946, 15142, 15338, 15533, 15728, 15923, 16118, 16313, 16508, 16702,
16897, 17091, 17285, 17479, 17672, 17866, 18059, 18253, 18446, 18638, 18831,
19024, 19216, 19408, 19600, 19792, 19983, 20175, 20366, 20557, 20748, 20938,
21129, 21319, 21509, 21699, 21889, 22078, 22267, 22456, 22645, 22833, 23022,
23210, 23398, 23586, 23773, 23960, 24147, 24334, 24521, 24707, 24893, 25079,
25265, 25450, 25635, 25820, 26005, 26189, 26373, 26557, 26741, 26925, 27108,
27291, 27473, 27656, 27838, 28020, 28201, 28383, 28564, 28745, 28925, 29105,
29285, 29465, 29645, 29824, 30003, 30181, 30360, 30538, 30715, 30893, 31070,
31247, 31424, 31600, 31776, 31952, 32127, 32302, 32477, 32651, 32826, 32999,
33173, 33346, 33519, 33692, 33864, 34036, 34208, 34379, 34550, 34721, 34891,
35061, 35231, 35400, 35569, 35738, 35906, 36074, 36242, 36409, 36576, 36743,
36909, 37075, 37241, 37406, 37571, 37736, 37900, 38064, 38227, 38390, 38553,
38716, 38878, 39039, 39201, 39362, 39522, 39682, 39842, 40002, 40161, 40319,
40478, 40636, 40793, 40950, 41107, 41263, 41419, 41575, 41730, 41885, 42040,
42194, 42347, 42501, 42653, 42806, 42958, 43110, 43261, 43412, 43562, 43712,
43862, 44011, 44160, 44308, 44456, 44603, 44750, 44897, 45043, 45189, 45335,
45480, 45624, 45768, 45912, 46055, 46198, 46340, 46482, 46624, 46765, 46906,
47046, 47186, 47325, 47464, 47602, 47740, 47878, 48015, 48151, 48288, 48423,
48558, 48693, 48828, 48961, 49095, 49228, 49360, 49492, 49624, 49755, 49886,
50016, 50146, 50275, 50403, 50532, 50660, 50787, 50914, 51040, 51166, 51291,
51416, 51541, 51665, 51788, 51911, 52033, 52155, 52277, 52398, 52518, 52639,
52758, 52877, 52996, 53114, 53231, 53348, 53465, 53581, 53696, 53811, 53926,
54040, 54153, 54266, 54379, 54491, 54602, 54713, 54823, 54933, 55043, 55152,
55260, 55368, 55475, 55582, 55688, 55794, 55899, 56004, 56108, 56212, 56315,
56417, 56519, 56621, 56722, 56822, 56922, 57022, 57120, 57219, 57316, 57414,
57510, 57606, 57702, 57797, 57892, 57986, 58079, 58172, 58264, 58356, 58447,
58538, 58628, 58718, 58807, 58895, 58983, 59070, 59157, 59243, 59329, 59414,
59499, 59583, 59666, 59749, 59831, 59913, 59994, 60075, 60155, 60235, 60313,
60392, 60470, 60547, 60624, 60700, 60775, 60850, 60924, 60998, 61071, 61144,
61216, 61288, 61359, 61429, 61499, 61568, 61637, 61705, 61772, 61839, 61905,
61971, 62036, 62100, 62164, 62228, 62291, 62353, 62414, 62475, 62536, 62596,
62655, 62714, 62772, 62829, 62886, 62942, 62998, 63053, 63108, 63162, 63215,
63268, 63320, 63371, 63422, 63473, 63522, 63571, 63620, 63668, 63715, 63762,
63808, 63854, 63899, 63943, 63987, 64030, 64073, 64115, 64156, 64197, 64237,
64276, 64315, 64353, 64391, 64428, 64465, 64501, 64536, 64571, 64605, 64638,
64671, 64703, 64735, 64766, 64796, 64826, 64855, 64884, 64912, 64939, 64966,
64992, 65018, 65043, 65067, 65091, 65114, 65136, 65158, 65179, 65200, 65220,
65239, 65258, 65276, 65294, 65311, 65327, 65343, 65358, 65372, 65386, 65400,
65412, 65424, 65436, 65446, 65457, 65466, 65475, 65483, 65491, 65498, 65505,
65511, 65516, 65520, 65524, 65528, 65531, 65533, 65534, 65535
};

