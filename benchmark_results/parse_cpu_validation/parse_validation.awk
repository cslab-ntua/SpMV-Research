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

    matrix_features["scircuit"]             = "5.607878455,4.39216211,61.94715601,0.2972525308,0.2787424191,0.1609197429,0.4197409764,0.803365,0.633019"
    matrix_features["mac_econ_fwd500"]      = "6.166532688,4.435865332,6.135290158,0.001907956522,0.00206892622,0.1672149757,0.4029767362,0.176686,0.330509"
    matrix_features["raefsky3"]             = "70.22490566,6.326999832,0.1391969736,0.0662003204,0.008402996808,0.05071248511,0.00610160059,1.916,0.96302"
    matrix_features["bbmat"]                = "45.72893867,38.39531373,1.755366813,0.02989415739,0.07411226278,0.0500688693,0.04267120883,1.262626,0.85373"
    matrix_features["conf5_4-8x8-15"]       = "39,0,0,0.2446905772,0.2500397592,0.004855366006,0.001675641851,1.441506,0.810948"
    matrix_features["mc2depi"]              = "3.994152047,0.07632277052,0.001464128789,0.001342169398,0.0002735273401,0.006482439685,0.00789767623,0.498297,0.998906"
    matrix_features["rma10"]                = "50.68860895,27.78059606,1.86060326,0.1877712645,0.2222071625,0.1291094336,0.184910451,1.719725,0.866408"
    matrix_features["cop20k_A"]             = "21.65432537,13.79266245,2.740592173,0.6230549795,0.3159608012,0.0002423827951,0.0001551766894,1.095827,0.6334"
    matrix_features["webbase-1M"]           = "3.105520472,25.34520973,1512.433913,0.1524629001,0.1243869793,0.009501850807,0.1021692672,0.315681,0.936095"
    matrix_features["cant"]                 = "64.16843605,14.05626099,0.215550897,0.008604097649,0.0006624492823,0.118684497,0.02442676465,1.61575,0.914729"
    matrix_features["pdb1HYS"]              = "119.305956,31.86038422,0.7098894878,0.1299377034,0.1991685923,0.1500344034,0.2167175011,1.837758,0.931726"
    matrix_features["TSOPF_RS_b300_c3"]     = "104.73798,102.4431672,0.9954557077,0.6066963064,0.1767735655,0.003794703704,0.004016927878,1.923459,0.992154"
    matrix_features["Chebyshev4"]           = "78.94424627,1061.43997,861.9001253,0.04535742434,0.1184167116,0.02043058444,0.06794422044,0.895986,0.993706"
    matrix_features["consph"]               = "72.125183,19.08019415,0.1230474105,0.06981133797,0.09789778393,0.02479497703,0.01711731501,1.71335,0.882633"
    matrix_features["shipsec1"]             = "55.46377614,11.07481064,0.8390381452,0.04587554507,0.01906937284,0.01307381958,0.02505676283,1.712385,0.873444"
    matrix_features["PR02R"]                = "50.81725958,19.6982847,0.8104085258,0.03958570835,0.06812315648,0.01317692339,0.03675759647,1.279542,0.86886"
    matrix_features["mip1"]                 = "155.7681567,350.7443175,425.2424452,0.5902849043,0.2608459076,0.005713332809,0.03343832666,1.924902,0.96147"
    matrix_features["rail4284"]             = "2633.994398,4209.259315,20.32958219,0.9555418052,0.1118968799,0.002581100147,0.00456847561,1.576267,0.269166"
    matrix_features["pwtk"]                 = "53.38899953,4.743895102,2.371481046,0.05932070192,0.2007594999,0.079323796,0.08958474906,1.878352,0.949809"
    matrix_features["crankseg_2"]           = "221.6369247,95.87570167,14.44417747,0.865768503,0.08105367823,0.004035441234,0.001742964722,1.731501,0.86687"
    matrix_features["Si41Ge41H72"]          = "80.86266894,126.9718576,7.186719641,0.1935377211,0.06005700215,0.002166066843,0.002863971545,1.269782,0.963174"
    matrix_features["TSOPF_RS_b2383"]       = "424.2174449,484.237499,1.317207865,0.4832654419,0.2083293262,0.02002676107,0.06755794394,1.979904,0.927411"
    matrix_features["in-2004"]              = "12.23295621,37.23001003,632.7797558,0.02150049625,0.102543014,0.2037970483,0.4017078357,1.507183,0.792656"
    matrix_features["Ga41As41H72"]          = "68.96214789,105.3875532,9.179497325,0.1722725043,0.05292462067,0.001480063236,0.001862032207,1.184016,0.969575"
    matrix_features["eu-2005"]              = "22.29737186,29.33341123,312.2656191,0.248901028,0.310854575,0.03460001589,0.1933491413,1.330828,0.760172"
    matrix_features["wikipedia-20051105"]   = "12.08147455,31.07497821,410.3736266,0.3400604749,0.3804982055,0.0004176705624,0.02362405218,0.067098,0.070929"
    matrix_features["rajat31"]              = "4.33182182,1.106157847,288.0238916,0.0009716598294,0.01891079113,0.01921252038,0.007118543221,0.553744,0.648584"
    matrix_features["ldoor"]                = "48.85772782,11.94657153,0.5760045224,0.2042067138,0.1961324819,0.005104674755,0.02812790585,1.79674,0.906047"
    matrix_features["circuit5M"]            = "10.709032,1356.616274,120504.8496,0.5025163836,0.2558205218,0.0005554600556,0.02468554043,1.065325,0.942768"
    matrix_features["bone010"]              = "72.63211422,15.81042955,0.1152091726,0.01817301802,0.005263120397,0.004305201544,0.001500221571,1.769845,0.915136"
    matrix_features["cage15"]               = "19.24389222,5.736719369,1.442333363,0.2119567644,0.130329138,0.0000345,0.0000954,0.197548,0.794106"
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
