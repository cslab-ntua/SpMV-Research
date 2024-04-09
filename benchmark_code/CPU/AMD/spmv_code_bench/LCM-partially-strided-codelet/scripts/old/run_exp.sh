#!/bin/bash
BINLIB=$1
TUNED=$2
THRDS=$3
MAT_DIR=$4
SPD_MAT_DIR=$5

export OMP_NUM_THREADS=$THRDS
export MKL_NUM_THREADS=$THRDS

header=1

MATS=($(ls  $MAT_DIR))
SPD_MATS=($(ls $SPD_MAT_DIR))

if [ "$TUNED" ==  1 ]; then
  for bp in {0,1}; do
    for mat in "${SPD_MATS[@]}"; do
      for k in {2,3,4,5,6,7,8,9}; do
        # for cparm in {1,2,3,4,5,10,20}; do
        if [ $header -eq 1 ]; then
          $BINLIB  -m $SPD_MAT_DIR/$mat/$mat.mtx -n SPTRS -s CSR -t $THRDS -c $k -d -p $bp -u 1
          header=0
        else
          $BINLIB  -m $SPD_MAT_DIR/$mat/$mat.mtx -n SPTRS -s CSR -t $THRDS -c $k -p $bp -u 1
        fi
      done
    done
  done
fi

MAT_SPD_NAME="bcsstk15/bcsstk15 bcsstk16/bcsstk16 bcsstk17/bcsstk17 bcsstk18/bcsstk18 bcsstk24/bcsstk24 bcsstk25/bcsstk25 bcsstk28/bcsstk28 bcsstk36/bcsstk36 bcsstk38/bcsstk38 crystm01/crystm01 crystm02/crystm02 crystm03/crystm03 ct20stif/ct20stif msc10848/msc10848 msc23052/msc23052 pwtk/pwtk finan512/finan512 nasa2910/nasa2910 nasa4704/nasa4704 nasasrb/nasasrb aft01/aft01 cfd1/cfd1 cfd2/cfd2 olafu/olafu raefsky4/raefsky4 qa8fm/qa8fm bodyy4/bodyy4 bodyy5/bodyy5 bodyy6/bodyy6 Andrews/Andrews nd3k/nd3k nd6k/nd6k nd12k/nd12k nd24k/nd24k af_shell3/af_shell3 af_shell4/af_shell4 af_shell7/af_shell7 af_shell8/af_shell8 Pres_Poisson/Pres_Poisson gyro_k/gyro_k gyro_m/gyro_m t2dah_e/t2dah_e audikw_1/audikw_1 bmw7st_1/bmw7st_1 bmwcra_1/bmwcra_1 crankseg_1/crankseg_1 crankseg_2/crankseg_2 hood/hood inline_1/inline_1 ldoor/ldoor m_t1/m_t1 oilpan/oilpan s3dkq4m2/s3dkq4m2 s3dkt3m2/s3dkt3m2 ship_001/ship_001 ship_003/ship_003 shipsec1/shipsec1 shipsec5/shipsec5 shipsec8/shipsec8 thread/thread vanbody/vanbody wathen100/wathen100 wathen120/wathen120 x104/x104 cvxbqp1/cvxbqp1 gridgena/gridgena jnlbrng1/jnlbrng1 minsurfo/minsurfo obstclae/obstclae torsion1/torsion1 Kuu/Kuu Muu/Muu bundle1/bundle1 thermal1/thermal1 thermal2/thermal2 ted_B/ted_B ted_B_unscaled/ted_B_unscaled G2_circuit/G2_circuit G3_circuit/G3_circuit apache1/apache1 apache2/apache2 gyro/gyro bone010/bone010 boneS01/boneS01 boneS10/boneS10 af_0_k101/af_0_k101 af_1_k101/af_1_k101 af_2_k101/af_2_k101 af_3_k101/af_3_k101 af_4_k101/af_4_k101 af_5_k101/af_5_k101 s1rmq4m1/s1rmq4m1 s2rmq4m1/s2rmq4m1 s3rmq4m1/s3rmq4m1 s1rmt3m1/s1rmt3m1 s2rmt3m1/s2rmt3m1 s3rmt3m1/s3rmt3m1 s3rmt3m3/s3rmt3m3 msdoor/msdoor Dubcova1/Dubcova1 Dubcova2/Dubcova2 Dubcova3/Dubcova3 BenElechi1/BenElechi1 parabolic_fem/parabolic_fem ecology2/ecology2 denormal/denormal tmt_sym/tmt_sym smt/smt cbuckle/cbuckle 2cubes_sphere/2cubes_sphere Trefethen_20000b/Trefethen_20000b Trefethen_20000/Trefethen_20000 thermomech_TC/thermomech_TC thermomech_TK/thermomech_TK thermomech_dM/thermomech_dM shallow_water1/shallow_water1 shallow_water2/shallow_water2 offshore/offshore pdb1HYS/pdb1HYS consph/consph cant/cant Serena/Serena Emilia_923/Emilia_923 Fault_639/Fault_639 Flan_1565/Flan_1565 Geo_1438/Geo_1438 Hook_1498/Hook_1498 StocF-1465/StocF-1465 Bump_2911/Bump_2911 Queen_4147/Queen_4147 PFlow_742/PFlow_742 bundle_adj/bundle_adj"


if [ "$TUNED" ==  5 ]; then
  for bp in {0,1}; do
    for mat in ${MAT_SPD_NAME}; do
      for k in {2,3,4,5,6,7,8,9}; do
        # for cparm in {1,2,3,4,5,10,20}; do
        if [ $header -eq 1 ]; then
          $BINLIB  -m $SPD_MAT_DIR/$mat.mtx -n SPTRS -s CSR -t $THRDS -c $k -d -p $bp -u 1
          header=0
        else
          $BINLIB  -m $SPD_MAT_DIR/$mat.mtx -n SPTRS -s CSR -t $THRDS -c $k -p $bp -u 1
        fi
      done
    done
  done
fi


prefer_fsc=( "" )
clt_min_widths=( 2 4 8 16 )
clt_max_distances=( 2 4 8 )

MAT_GRP0="2D_27628_bjtcai 2D_54019_highK 3D_28984_Tetra 3D_51448_3D a0nsdsil a2nnsnsl a5esindl ABACUS_shell_ud abtaha2 ACTIVSg10K ACTIVSg70K af23560 af_shell1 af_shell10 af_shell2 af_shell5 af_shell6 af_shell9 airfoil_2d ak2010 al2010 analytics appu ar2010 as-caida ASIC_100k ASIC_100ks ASIC_320k ASIC_320ks ASIC_680k ASIC_680ks astro-ph atmosmodd atmosmodj atmosmodl atmosmodm aug3dcqp av41092 az2010 barrier2-1 barrier2-10 barrier2-11 barrier2-12 barrier2-2 barrier2-3 barrier2-4 barrier2-9 bas1lp Baumann bauru5727 baxter bayer01 bbmat bcircuit bcsstk35 bcsstk37 bcsstk39 bcsstm36 benzene blockqp1 bloweya bloweybl bmw3_2 boyd1 boyd2 brainpc2 bratu3d c-39 c-41 c-42 c-43 c-45 c-46 c-47 c-48 c-49 c-50 c-51 c-52 c-53 c-54 c-55 c-56 c-57 c-58 c-59 c-60 c-61 c-62 c-62ghs c-63 c-64 c-64b c-65 c-66 c-66b c-67 c-67b c-68 c-69 c-70 c-71 c-72 c-73 c-73b c8_mat11 c8_mat11_I ca2010 cage10 cage11 cage12 cage13 cage14 cage15 CAG_mat1916 cari case39 case9 cavity16 cavity17 cavity18 cavity19 cavity20 cavity21 cavity22 cavity23 cavity24 cavity25 cavity26 c-big ch7-7-b5 ch7-8-b3 ch7-8-b4 ch7-8-b5 ch7-9-b3 ch7-9-b4 ch7-9-b5 ch8-8-b3 ch8-8-b4 ch8-8-b5 Chebyshev4 chem_master1 chipcool0 chipcool1 circuit_4 circuit5M circuit5M_dc cis-n4c6-b4 ckt11752_dc_1 ckt11752_tr_0 CO co2010 co9 coater2 cond-mat-2003 cond-mat-2005 cont11_l cont1_l cont-201 cont-300 cop20k_A copter2 CoupCons3D crashbasis crystk01 crystk02 crystk03 ct2010 Cube_Coup_dt0 Cube_Coup_dt6 CurlCurl_0 CurlCurl_1 CurlCurl_2 CurlCurl_3 CurlCurl_4 cvxqp3 cyl6 cz10228 cz20468 cz40948 D6-6 darcy003 dawson5 dbic1 dbir1 dbir2 dc1 dc2 dc3 de2010 degme Delor295K Delor338K Delor64K deltaX dgreen dielFilterV2real dielFilterV3real dixmaanl d_pretok e18 e40r0100 EAT_RS EAT_SR ecl32 ecology1 engine epb2 epb3 ESOC EternityII_A EternityII_E EternityII_Etilde ex11 ex19 ex35 ex40 exdata_1 F1 F2 f855_mat9 f855_mat9_I Fashion_MNIST_norm_10NN FEM_3D_thermal1 FEM_3D_thermal2 filter3D fl2010 foldoc fome12 fome13 fome20 fome21 fp Franz11 Franz8 Freescale1 Freescale2 FullChip"
#fxm3_16 fxm4_6  FX_March2010 
MAT_GRP1="fxm3_16 fxm4_6 FX_March2010 g7jac040 g7jac040sc g7jac050sc g7jac060 g7jac060sc g7jac080 g7jac080sc g7jac100 g7jac100sc g7jac120 g7jac120sc g7jac140 g7jac140sc g7jac160 g7jac160sc g7jac180 g7jac180sc g7jac200 g7jac200sc Ga10As10H30 Ga19As19H42 ga2010 Ga3As3H12 Ga41As41H72 GaAsH6 GAP-road garon2 gas_sensor Ge87H76 Ge99H100 gen4 GL7d13 GL7d14 GL7d15 GL7d16 GL7d17 GL7d18 GL7d19 GL7d20 GL7d21 GL7d22 GL7d23 GL7d24 goodwin Goodwin_023 Goodwin_030 Goodwin_040 Goodwin_054 Goodwin_071 Goodwin_095 Goodwin_127 graham1 graphics gsm_106857 GT01R H2O Hamrle3 hangGlider_4 hangGlider_5 har_10NN Hardesty1 Hardesty2 Hardesty3 hcircuit heart1 heart2 heart3 helm2d03 helm3d01 hi2010 HTC_336_4438 HTC_336_9129 human_gene1 human_gene2 HV15R hvdc1 hvdc2 ia2010 ibm_matrix_2 id2010 ifiss_mat IG5-14 IG5-15 IG5-16 IG5-17 IG5-18 igbt3 il2010 Ill_Stokes image_interp imagesensor in2010 indianpines_10NN inlet internet invextr1_new jan99jac060 jan99jac060sc jan99jac080 jan99jac080sc jan99jac100 jan99jac100sc jan99jac120 jan99jac120sc JapaneseVowelsSmall_10NN JP juba40k k1_san k3plates k49_norm_10NN karted Kemelmacher kkt_power kmnist_norm_10NN kron_g500-logn16 kron_g500-logn17 kron_g500-logn18 kron_g500-logn19 kron_g500-logn20 kron_g500-logn21 ks2010 ky2010 la2010 laminar_duct3D landmark language largebasis LargeRegFile LeGresley_87936 lhr07 lhr07c lhr10 lhr10c lhr11 lhr11c lhr14 lhr14c lhr17 lhr17c lhr34 lhr34c lhr71 lhr71c li Lin Linux_call_graph Long_Coup_dt0 Long_Coup_dt6 lowThrust_10 lowThrust_11 lowThrust_12 lowThrust_13 lowThrust_4 lowThrust_5 lowThrust_6 lowThrust_7 lowThrust_8 lowThrust_9 lp_cre_b lp_cre_d lp_fit2d lp_ken_18 lpl3 lp_maros_r7 lp_nug20 lp_nug30 lp_osa_07 lp_osa_14 lp_osa_30 lp_osa_60 lp_pds_10 lp_pds_20 lung2 m133-b3 ma2010 mac_econ_fwd500 majorbasis Maragal_6 Maragal_7 Maragal_8 marine1 mario001 mario002 mark3jac040 mark3jac040sc mark3jac060 mark3jac060sc mark3jac080 mark3jac080sc mark3jac100 mark3jac100sc mark3jac120 mark3jac120sc mark3jac140 mark3jac140sc matrix_9 matrix-new_3 mawi_201512012345 mawi_201512020000 mawi_201512020030 mawi_201512020130 mawi_201512020330 mc2depi md2010 me2010 memchip mesh_deform mhd4800a mi2010 mixtank_new mk12-b3 mk12-b4 mk13-b5 ML_Geer ML_Laplace mn2010 mnist_test_norm_10NN mo2010 mod2 model10 mosfet2 mouse_gene mri1 mri2 ms2010 mt2010 mult_dcop_01 mult_dcop_02 mult_dcop_03 n4c6-b10 n4c6-b11 n4c6-b12 n4c6-b4 n4c6-b5 n4c6-b6 n4c6-b7 n4c6-b8 n4c6-b9 Na5 nc2010 ncvxbqp1 ncvxqp3 ncvxqp5 ncvxqp7 nd2010 ne2010 nemeth01 nemeth02 nemeth03 nemeth04 nemeth05 nemeth06 nemeth07 nemeth08 nemeth09 nemeth10 nemeth11 nemeth12 nemeth13 nemeth14 nemeth15 nemeth16 nemeth17 nemeth18 nemeth19 nemeth20 nemeth21 nemeth22 nemeth23 nemeth24 nemeth25 nemeth26"

# nlpkkt200 nlpkkt240 
MAT_GRP2="nemsemm1 nemsemm2 nemswrld neos neos1 neos2 neos3 nh2010 nj2010 nlpkkt120 nlpkkt160 nlpkkt80 nm2010 nmos3 ns3Da nsct nsir nug08-3rd nv1 nv2 nv2010 nxp1 ny2010 oh2010 ohne2 ok2010 olesnik0 onetone1 onetone2 OPF_10000 OPF_3754 OPF_6000 or2010 p010 pa2010 para-10 para-4 para-5 para-6 para-7 para-8 para-9 patents_main pds-100 pds-30 pds-40 pds-50 pds-60 pds-70 pds-80 pds-90 piston pltexpa poisson3Da poisson3Db power197k power9 PR02R pre2 psmigr_1 psmigr_2 psmigr_3 psse0 psse2 qa8fk r05 radiation raefsky1 raefsky2 raefsky3 raefsky5 raefsky6 rail_20209 rail2586 rail4284 rail507 rail516 rail582 rail_79841 Raj1 rajat15 rajat16 rajat17 rajat18 rajat20 rajat21 rajat22 rajat23 rajat24 rajat25 rajat26 rajat28 rajat29 rajat30 rajat31 rat rel8 rel9 relat8 relat9 Reuters911 ri2010 rim rlfddd rlfdual rlfprim RM07R rma10 route Rucci1 sc2010 sc205-2r scagr7-2r scfxm1-2b scfxm1-2r scircuit scsd8-2b scsd8-2c scsd8-2r sctap1-2r sd2010 sgpf5y6 shar_te2-b2 shar_te2-b3 shermanACb shyy161 Si10H16 Si34H36 Si41Ge41H72 Si5H12 Si87H76 SiH4 SiNa sinc12 sinc15 sinc18 SiO SiO2 sls sme3Da sme3Db sme3Dc soc-sign-epinions soc-sign-Slashdot081106 soc-sign-Slashdot090216 soc-sign-Slashdot090221 south31 spal_004 sparsine specular spmsrtls ss ss1 stat96v1 stat96v2 stat96v3 stat96v4 stat96v5 std1_Jac2 std1_Jac2_db std1_Jac3 std1_Jac3_db stokes stokes128 stokes64 stokes64s stomach stormG2_1000 stormg2-125 sx-askubuntu sx-mathoverflow sx-stackoverflow sx-superuser t2dah t2dah_a t2em t3dh t3dh_a t3dh_e t3dl t3dl_a t520 ted_A ted_A_unscaled TEM152078 TEM181302 TEM27623 test1 TF16 TF17 TF18 TF19 thermomech_dK tmt_unsym tn2010 tomographic1 torso1 torso2 torso3 tp-6 trans4 trans5 transient Transport Trec12 Trec13 Trec14 TSC_OPF_1047 TSC_OPF_300 TSOPF_FS_b162_c1 TSOPF_FS_b162_c3 TSOPF_FS_b162_c4 TSOPF_FS_b300 TSOPF_FS_b300_c1 TSOPF_FS_b300_c2 TSOPF_FS_b300_c3 TSOPF_FS_b39_c19 TSOPF_FS_b39_c30 TSOPF_FS_b39_c7 TSOPF_FS_b9_c6 TSOPF_RS_b162_c1 TSOPF_RS_b162_c3 TSOPF_RS_b162_c4 TSOPF_RS_b2052_c1 TSOPF_RS_b2383 TSOPF_RS_b2383_c1 TSOPF_RS_b300_c1 TSOPF_RS_b300_c2 TSOPF_RS_b300_c3 TSOPF_RS_b39_c19 TSOPF_RS_b39_c30 TSOPF_RS_b39_c7 TSOPF_RS_b678_c1 TSOPF_RS_b678_c2 ts-palko tube2 turon_m twotone tx2010 ulevimin ut2010 va2010 vas_stokes_1M vas_stokes_2M vas_stokes_4M venkat01 venkat25 venkat50 vibrobox viscoplastic2 viscorocks vt2010 wa2010 wang3 wang4 water_tank watson_1 watson_2 webbase-1M wi2010 wiki-RfA wiki-talk-temporal windtunnel_evap2d windtunnel_evap3d Wordnet3 world worms20_10NN wv2010 wy2010 xenon1 xenon2 Zd_Jac2 Zd_Jac2_db Zd_Jac3 Zd_Jac3_db Zd_Jac6 Zd_Jac6_db Zhao1 Zhao2"


### SPMV
if [ "$TUNED" == 3 ]; then
  for mat in ${MAT_GRP2}; do
    for md in "${clt_max_distances[@]}"; do
      for mw in "${clt_min_widths[@]}"; do
        for pf in "${prefer_fsc[@]}"; do
          if [ $header -eq 1 ]; then
            $BINLIB  -m $MAT_DIR/$mat/$mat.mtx -n SPMV -s CSR -t $THRDS $pf --clt_width=$mw --col_th=$md -d
            header=0
          else
            $BINLIB  -m $MAT_DIR/$mat/$mat.mtx -n SPMV -s CSR -t $THRDS $pf --clt_width=$mw --col_th=$md
          fi
        done
      done
    done
  done
fi


M_TILE_SIZES=( 4 8 16 32 )
N_TILE_SIZES=( 4 )
B_MAT_COL=( 4 )
### SPMM
if [ "$TUNED" == 4 ]; then
  for mat in ${MAT_GRP0}; do
    for md in "${clt_max_distances[@]}"; do
      for mw in "${clt_min_widths[@]}"; do
        for mtile in "${M_TILE_SIZES[@]}"; do
          for ntile in "${N_TILE_SIZES[@]}"; do
            for bcol in "${B_MAT_COL[@]}"; do
              if [ "$ntile" -gt "$bcol" ]; then
                continue
              fi
              if [ $header -eq 1 ]; then
                $BINLIB -m $MAT_DIR/$mat/$mat.mtx -n SPMM -s CSR -t $THRDS --m_tile_size=$mtile --n_tile_size=$ntile --b_matrix_columns=$bcol -d --clt_width=$mw --col_th=$md
                header=0
              else
                $BINLIB -m $MAT_DIR/$mat/$mat.mtx -n SPMM -s CSR -t $THRDS --b_matrix_columns=$bcol --m_tile_size=$mtile --n_tile_size=$ntile --clt_width=$mw --col_th=$md
              fi
            done
          done
        done
      done
    done
  done
M_TILE_SIZES=( 4 8 16 32 )
N_TILE_SIZES=( 4 8 16 32 )
B_MAT_COL=( 4 16 64 128 )
### SPMM
if [ "$TUNED" == 4 ]; then
  for mat in  ${MAT_GRP0}; do
        for mtile in "${M_TILE_SIZES[@]}"; do
          for ntile in "${N_TILE_SIZES[@]}"; do
            for bcol in "${B_MAT_COL[@]}"; do
              if [ "$ntile" -gt "$bcol" ]; then
                continue
              fi
              if [ $header -eq 1 ]; then
                $BINLIB -m $MAT_DIR/$mat/$mat.mtx -n SPMM -s CSR -t $THRDS --m_tile_size=$mtile --n_tile_size=$ntile --b_matrix_columns=$bcol -d --clt_width=$mw --col_th=$md
                header=0
              else
                $BINLIB -m $MAT_DIR/$mat/$mat.mtx -n SPMM -s CSR -t $THRDS --b_matrix_columns=$bcol --m_tile_size=$mtile --n_tile_size=$ntile --clt_width=$mw --col_th=$md
              fi
            done
          done
    done
  done
fi





M_TILE_SIZES=( 4 8 16 32 )
N_TILE_SIZES=( 4 8 16 32 )
B_MAT_COL=( 256 )
### SPMM
if [ "$TUNED" == 5 ]; then
  for mat in "${MATS[@]}"; do
    for md in "${clt_max_distances[@]}"; do
      for mw in "${clt_min_widths[@]}"; do
        for mtile in "${M_TILE_SIZES[@]}"; do
          for ntile in "${N_TILE_SIZES[@]}"; do
            for bcol in "${B_MAT_COL[@]}"; do
              if [ "$ntile" -gt "$bcol" ]; then
                continue
              fi
              if [ $header -eq 1 ]; then
                $BINLIB -m $MAT_DIR/$mat/$mat.mtx -n SPMM -s CSR -t $THRDS --m_tile_size=$mtile --n_tile_size=$ntile --b_matrix_columns=$bcol -d --clt_width=$mw --col_th=$md
                header=0
              else
                $BINLIB -m $MAT_DIR/$mat/$mat.mtx -n SPMM -s CSR -t $THRDS --b_matrix_columns=$bcol --m_tile_size=$mtile --n_tile_size=$ntile --clt_width=$mw --col_th=$md
              fi
            done
          done
        done
      done
    done
  done
fi
