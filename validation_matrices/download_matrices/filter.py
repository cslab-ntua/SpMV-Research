def extract_lines_with_matrices(original_script, matrix_names):
    extracted_lines = []
    for matrix_name in matrix_names:
        with open(original_script, 'r') as file:
            for line in file:        
                if matrix_name in line:
                    extracted_lines.append(line)
                    break  # Break if a matrix name is found in the line
    return extracted_lines

def create_new_bash_script(extracted_lines, output_script):
    with open(output_script, 'w') as file:
        file.write("# build the sort-mtx executable, that will be used to sort matrix elements (by row and by column)\n")
        file.write("g++ -Wall -O3 sort-mtx.cpp -o sort-mtx\n\n")
        for line in extracted_lines:
            file.write(line)
    print('Finished writing new bash script to', output_script)

# List of matrix names to extract
# matrix_names_to_extract = [
#  'spal_004', 'ldoor', 'dielFilterV2real', 'af_shell10', 'nv2', 'boneS10', 'circuit5M', 'Hook_1498', 'Geo_1438', 'Serena', 'vas_stokes_2M', 'bone010', 'audikw_1', 'Long_Coup_dt0', 'Long_Coup_dt6', 'dielFilterV3real', 'nlpkkt120', 'cage15', 'ML_Geer', 'Flan_1565', 'Cube_Coup_dt0', 'Cube_Coup_dt6', 'Bump_2911', 'vas_stokes_4M', 'nlpkkt160', 'HV15R', 'Queen_4147', 'stokes', 'nlpkkt200'
# ]
matrix_names_to_extract = [
    'cit-Patents', 'human_gene2', 'GL7d21', 'Ga41As41H72', 'great-britain_osm', 'hugetric-00000', 'eu-2005', 'Freescale1', 'wikipedia-20051105', 'circuit5M_dc', 'bundle_adj', 'msdoor', 'fem_hifreq_circuit', 'kron_g500-logn18', 'StocF-1465', 'rajat31', 'gsm_106857', 'hugetric-00010', 'M6', 'CoupCons3D', '12month1', 'as-Skitter', '333SP', 'hugetric-00020', 'AS365', 'Transport', 'Freescale2', 'human_gene1', 'NLR', 'GL7d17', 'delaunay_n22', 'F1', 'rel9', 'CurlCurl_4', 'FullChip', 'cage14', 'ML_Laplace', 'germany_osm', 'nd24k', 'Fault_639', 'mouse_gene', 'nlpkkt80', 'wiki-topcats', 'asia_osm', 'adaptive', 'rgg_n_2_21_s0', 'GL7d20', 'coPapersDBLP', 'soc-Pokec', 'coPapersCiteseer', 'dielFilterV3clx', 'mycielskian16', 'vas_stokes_1M', 'ss', 'packing-500x100x100-b050', 'GL7d18', 'inline_1', 'sx-stackoverflow', 'PFlow_742', 'RM07R', 'GL7d19', 'wikipedia-20060925', 'road_central', 'dgreen', 'hugetrace-00010', 'wikipedia-20061104', 'Emilia_923', 'relat9', 'Hardesty3', 'kron_g500-logn19', 'mawi_201512012345', 'spal_004', 'wikipedia-20070206', 'ldoor', 'dielFilterV2real', 'delaunay_n23', 'af_shell10', 'nv2', 'hugetrace-00020', 'boneS10', 'wb-edu', 'hugebubbles-00000', 'circuit5M', 'Hook_1498', 'rgg_n_2_22_s0', 'Geo_1438', 'hugebubbles-00010', 'Serena', 'GAP-road', 'road_usa', 'vas_stokes_2M', 'soc-LiveJournal1', 'hugebubbles-00020', 'com-LiveJournal', 'bone010', 'audikw_1', 'ljournal-2008', 'mawi_201512020000', 'channel-500x100x100-b050', 'Long_Coup_dt0', 'Long_Coup_dt6', 'kron_g500-logn20', 'dielFilterV3real', 'nlpkkt120', 'mycielskian17', 'cage15', 'delaunay_n24', 'ML_Geer', 'hollywood-2009', 'Flan_1565', 'europe_osm', 'Cube_Coup_dt0', 'Cube_Coup_dt6', 'Bump_2911', 'rgg_n_2_23_s0', 'vas_stokes_4M', 'kmer_V2a', 'kmer_U1a', 'mawi_201512020030', 'kron_g500-logn21', 'indochina-2004', 'nlpkkt160', 'com-Orkut', 'rgg_n_2_24_s0', 'HV15R', 'mycielskian18', 'uk-2002', 'mawi_201512020130', 'Queen_4147'
]
print('Gotta download', len(matrix_names_to_extract), 'matrices')

# Path to the original bash script
original_bash_script_path = 'SuiteSparseCollection.sh'

# Path to the new bash script to create
new_bash_script_path = 'filtered_collection.sh'

# Extract lines containing the specified matrix names
extracted_lines = extract_lines_with_matrices(original_bash_script_path, matrix_names_to_extract)

# Create a new bash script with the extracted lines
create_new_bash_script(extracted_lines, new_bash_script_path)

