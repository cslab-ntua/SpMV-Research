#!/usr/bin/awk -f

BEGIN {
    num_fields = 0
    j = 4
    for (i=0;j<=4096;i++)
    {
        mb_list[i] = j
        j *= 2
    }
    mb_list_n = i

    matrix_features["scircuit"]             = "5.607878,4.392162,61.947156,0.297253,0.278742,0.16092,0.419741,0.803365,0.633019"
    matrix_features["mac_econ_fwd500"]      = "6.166533,4.435865,6.13529,0.001908,0.002069,0.167215,0.402977,0.176686,0.330509"
    matrix_features["raefsky3"]             = "70.224906,6.327,0.139197,0.0662,0.008403,0.050712,0.006102,1.916,0.96302"
    matrix_features["rgg_n_2_17_s0"]        = "11.119888,3.34422,1.518011,0.007427,0.04219,2.23E-02,7.73E-03,0.033953,0.051741"
    matrix_features["bbmat"]                = "45.728939,38.395314,1.755367,0.029894,0.074112,0.050069,0.042671,1.262626,0.85373"
    matrix_features["appu"]                 = "132.364571,36.494459,1.221138,0.983543,0.0129,9.60E-03,2.61E-03,0.02021,0.036817"
    matrix_features["mc2depi"]              = "3.994152,0.076323,0.001464,0.001342,0.000274,0.006482,0.007898,0.498297,0.998906"
    matrix_features["rma10"]                = "50.688609,27.780596,1.860603,0.187771,0.222207,0.129109,0.18491,1.719725,0.866408"
    matrix_features["cop20k_A"]             = "21.654325,13.792662,2.740592,0.623055,0.315961,0.000242,0.000155,1.095827,0.6334"
    matrix_features["thermomech_dK"]        = "13.930519,1.430513,0.435697,0.44461,0.219147,2.63E-04,3.45E-04,1.009616,0.543671"
    matrix_features["webbase-1M"]           = "3.10552,25.34521,1512.433913,0.152463,0.124387,0.009502,0.102169,0.315681,0.936095"
    matrix_features["cant"]                 = "64.168436,14.056261,0.215551,0.008604,0.000662,0.118684,0.024427,1.61575,0.914729"
    matrix_features["ASIC_680k"]            = "5.66992,659.807358,69710.56399,0.374662,0.22867,3.30E-05,9.97E-04,0.669008,0.825474"
    matrix_features["roadNet-TX"]           = "2.758265,1.037025,3.35056,0.004945,0.043172,1.17E-01,2.59E-01,0.161729,0.300952"
    matrix_features["pdb1HYS"]              = "119.305956,31.860384,0.709889,0.129938,0.199169,0.150034,0.216718,1.837758,0.931726"
    matrix_features["TSOPF_RS_b300_c3"]     = "104.73798,102.443167,0.995456,0.606696,0.176774,0.003795,0.004017,1.923459,0.992154"
    matrix_features["Chebyshev4"]           = "78.944246,1061.43997,861.900125,0.045357,0.118417,0.020431,0.067944,0.895986,0.993706"
    matrix_features["consph"]               = "72.125183,19.080194,0.123047,0.069811,0.097898,0.024795,0.017117,1.71335,0.882633"
    matrix_features["com-Youtube"]          = "5.265046,50.754343,5460.300863,0.1949,0.281164,3.68E-03,6.93E-02,0.282071,0.439166"
    matrix_features["rajat30"]              = "9.589184,784.579857,47421.80439,0.376075,0.268163,1.27E-02,1.24E-01,0.790904,0.658327"
    matrix_features["radiation"]            = "34.233756,15.156711,101.179847,0.596561,0.145678,3.89E-04,1.14E-03,0.026531,0.135845"
    matrix_features["Stanford_Berkeley"]    = "11.095794,284.832088,7519.687595,0.007119,0.045395,2.51E-01,4.46E-01,1.326604,0.652664"
    matrix_features["shipsec1"]             = "55.463776,11.074811,0.839038,0.045876,0.019069,0.013074,0.025057,1.712385,0.873444"
    matrix_features["PR02R"]                = "50.81726,19.698285,0.810409,0.039586,0.068123,0.013177,0.036758,1.279542,0.86886"
    matrix_features["CurlCurl_2"]           = "11.061957,0.921613,0.175199,0.027978,0.005542,1.26E-03,1.10E-02,0.715522,0.524131"
    matrix_features["gupta3"]               = "555.528034,1233.520259,25.410908,0.571842,0.276519,6.43E-02,8.62E-02,1.901659,0.976749"
    matrix_features["mip1"]                 = "155.768157,350.744318,425.242445,0.590285,0.260846,0.005713,0.033438,1.924902,0.96147"
    matrix_features["rail4284"]             = "2633.994398,4209.259315,20.329582,0.955542,0.111897,0.002581,0.004568,1.576267,0.269166"
    matrix_features["pwtk"]                 = "53.389,4.743895,2.371481,0.059321,0.200759,0.079324,0.089585,1.878352,0.949809"
    matrix_features["crankseg_2"]           = "221.636925,95.875702,14.444177,0.865769,0.081054,0.004035,0.001743,1.731501,0.86687"
    matrix_features["Si41Ge41H72"]          = "80.862669,126.971858,7.18672,0.193538,0.060057,0.002166,0.002864,1.269782,0.963174"
    matrix_features["TSOPF_RS_b2383"]       = "424.217445,484.237499,1.317208,0.483265,0.208329,0.020027,0.067558,1.979904,0.927411"
    matrix_features["in-2004"]              = "12.232956,37.23001,632.779756,0.0215,0.102543,0.203797,0.401708,1.507183,0.792656"
    matrix_features["Ga41As41H72"]          = "68.962148,105.387553,9.179497,0.172273,0.052925,0.00148,0.001862,1.184016,0.969575"
    matrix_features["eu-2005"]              = "22.297372,29.333411,312.265619,0.248901,0.310855,0.0346,0.193349,1.330828,0.760172"
    matrix_features["wikipedia-20051105"]   = "12.081475,31.074978,410.373626,0.34006,0.380498,0.000418,0.023624,0.067098,0.070929"
    matrix_features["kron_g500-logn18"]     = "80.741531,453.732304,607.893579,0.551851,0.417599,3.27E-04,2.18E-03,0.019866,0.020173"
    matrix_features["rajat31"]              = "4.331822,1.106158,288.023892,0.000972,0.018911,1.92E-02,7.12E-03,0.553744,0.648584"
    matrix_features["human_gene1"]          = "1107.106,1409.121606,6.170948,0.931477,0.187447,5.70E-02,1.28E-01,0.482284,0.282063"
    matrix_features["delaunay_n22"]         = "5.999979,1.336383,2.833346,0.419681,0.266977,5.83E-02,1.75E-01,0.493732,0.480906"
    matrix_features["GL7d20"]               = "20.794509,4.140267,17.995399,0.537972,0.191225,3.10E-05,3.29E-04,0.000121,0.000141"
    matrix_features["sx-stackoverflow"]     = "13.925354,137.849858,2738.463634,0.240155,0.309139,1.48E-04,9.75E-03,0.003092,0.11658"
    matrix_features["dgreen"]               = "31.867005,11.448452,4.868138,0.549954,0.155339,5.10E-05,2.00E-05,0.050171,0.167349"
    matrix_features["mawi_201512012345"]    = "2.048355,3805.811884,8006372.085,0.009047,0.072803,2.42E-03,6.04E-02,0.883952,0.887449"
    matrix_features["ldoor"]                = "48.857728,11.946572,0.576005,0.204207,0.196132,0.005105,0.028128,1.79674,0.906047"
    matrix_features["dielFilterV2real"]     = "41.935894,16.14859,1.623051,0.510504,0.183465,8.30E-05,4.90E-05,0.848705,0.549122"
    matrix_features["circuit5M"]            = "10.709032,1356.616274,120504.8496,0.502516,0.255821,0.000555,0.024686,1.065325,0.942768"
    matrix_features["soc-LiveJournal1"]     = "14.232648,36.08028,1424.80633,0.346982,0.312234,9.25E-03,1.15E-01,0.284272,0.28178"
    matrix_features["bone010"]              = "72.632114,15.81043,0.115209,0.018173,0.005263,0.004305,0.0015,1.769845,0.915136"
    matrix_features["audikw_1"]             = "82.284898,42.445255,3.19275,0.62315,0.215009,1.58E-04,9.60E-05,1.581103,0.819704"
    matrix_features["cage15"]               = "19.243892,5.736719,1.442333,0.211957,0.130329,3.40E-05,9.50E-05,0.197548,0.794106"
    matrix_features["kmer_V2a"]             = "2.129589,0.673107,17.313397,0.337063,0.242967,1.78E-04,1.81E-02,0.057325,0.116365"
}

BEGINFILE {
}

# /^File:/ {
#     matrix = $2
#     sub(".*/", "", matrix)
#     sub("\\..*$", "", matrix)
# }

function find_mem_range(mem, str)
{
    str = ""
    for (i=0;i<mb_list_n;i++)
        if (mem < mb_list[i])
            break;
    if (i == 0)
        str = sprintf("[<%g]", mb_list[0])
    else if (i >= mb_list_n)
        str = sprintf("[>%g]", mb_list[mb_list_n - 1])
    else
        str = sprintf("[%g-%g]", mb_list[i-1], mb_list[i])
    return str
}

# Variables given as a -v command line option: THREADS, IMPLEMENTATION
#
#     scircuit,unused,unused,0,0,
#     170998,170998,958936,
#     0.003279499441349471,11.6265,[8-16],
#     5.607878455,4.39216211,
#     0.2972525308,0.2787424191,
#     0.1609197429,0.4197409764,
#     MKL IE,2.7128513999417222e-05,70.6958,0,0,
#     HawkAmdRome
#
#     std::cout << "synthetic" << "," << AM->distribution << "," << AM->placement << "," << AM->seed
#         << "," << AM->nr_rows << "," << AM->nr_cols << "," << AM->nr_nzeros
#         << "," << AM->density << "," << AM->mem_footprint << "," << AM->mem_range
#         << "," << AM->avg_nnz_per_row << "," << AM->std_nnz_per_row
#         << "," << AM->avg_bw << "," << AM->std_bw
#         << "," << AM->avg_bw_scaled << "," << AM->std_bw_scaled
#         << "," << AM->avg_sc << "," << AM->std_sc
#         << "," << AM->avg_sc_scaled << "," << AM->std_sc_scaled
#         << "," << AM->skew
#         << "," << AM->avg_num_neighbours << "," << AM->cross_row_similarity
#         << "," << format_name <<  "," << time << "," << gflops << "," << W_avg << "," << J_estimated

# ^   Indicates the beginning of the line
# $   Indicates the end of a line
# \A  Denotes the beginning of a string
# \z  Denotes the end of a string
# \b  Marks a word boundary
# The ‘^‘ symbol indicates the start of a line, and the ‘|‘ symbol indicates a logical OR statement.

/^(\/|..)/{
    num_fields = split($0, tok, ",")

    matrix = tok[1]
    sub(".*/", "", matrix)
    sub("\\..*$", "", matrix)

    threads = tok[2]
    m = tok[3]
    n = tok[4]
    nnz = tok[5]
    time = tok[6]
    gflops = tok[7]
    mem_footprint = tok[8]
    w_avg = tok[9]
    j_estim = tok[10]
    format = tok[11]
    # printf("threads %s, m %s, n %s, nnz %s, time %s, gflops %s, mem_footprint %s, w_avg %s, j_estim %s, format %s\n", threads, m, n, nnz, time, gflops, mem_footprint, w_avg, j_estim, format)

    if (threads != THREADS)
        next
    if(format != IMPLEMENTATION)
        # printf("\t%s vs %s\n", format, IMPLEMENTATION)
        next

    density = nnz / (m * n) * 100 
    mem_range = find_mem_range(mem_footprint)

    distribution = "unused"
    placement = "unused"
    seed = 0

    split(matrix_features[matrix], feat, ",")
    i = 1
    avg_nnz_per_row = feat[i++]
    std_nnz_per_row = feat[i++]
    skew = feat[i++]
    avg_bw_scaled = feat[i++]
    std_bw_scaled = feat[i++]
    avg_sc = feat[i++]
    std_sc = feat[i++]

    avg_num_neighbours = feat[i++]
    cross_row_similarity = feat[i++]

    avg_bw = avg_bw_scaled * n
    std_bw = std_bw_scaled * n
    avg_sc_scaled = avg_sc * n
    std_sc_scaled = std_sc * n

    str = ""
    str = sprintf("%s,%s,%s,%d", matrix, distribution, placement, seed)
    str = sprintf("%s,%d,%d,%d", str, m, n, nnz)
    str = sprintf("%s,%g,%g,%s", str, density, mem_footprint, mem_range)

    str = sprintf("%s,%g,%g", str, avg_nnz_per_row, std_nnz_per_row)
    str = sprintf("%s,%g,%g", str, avg_bw, std_bw)
    str = sprintf("%s,%g,%g", str, avg_bw_scaled, std_bw_scaled)
    str = sprintf("%s,%g,%g", str, avg_sc, std_sc)
    str = sprintf("%s,%g,%g", str, avg_sc_scaled, std_sc_scaled)
    str = sprintf("%s,%g", str, skew)
    str = sprintf("%s,%g,%g", str, avg_num_neighbours, cross_row_similarity)

    # str = sprintf("%s,%s,%g,%g,0,0,HawkAmdRome", str, IMPLEMENTATION, time, gflops)
    str = sprintf("%s,%s,%g,%g,%g,%g,%s", str, IMPLEMENTATION, time, gflops, w_avg, j_estim, DEVICE)
    printf("%s\n", str)
}

END {
}
