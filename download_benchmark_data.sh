#/bin/bash

# We use 16 megabyte prefixes of the texts.
LENGTH=16777216

curl http://pizzachili.dcc.uchile.cl/texts/code/sources.50MB.gz | zcat | head -c $LENGTH > data_texts_sources
curl http://pizzachili.dcc.uchile.cl/texts/music/pitches.50MB.gz | zcat | head -c $LENGTH > data_texts_pitches
curl http://pizzachili.dcc.uchile.cl/texts/protein/proteins.50MB.gz | zcat | head -c $LENGTH > data_texts_proteins
curl http://pizzachili.dcc.uchile.cl/texts/dna/dna.50MB.gz | zcat | head -c $LENGTH > data_texts_dna
curl http://pizzachili.dcc.uchile.cl/texts/nlang/english.50MB.gz | zcat | head -c $LENGTH > data_texts_english
curl http://pizzachili.dcc.uchile.cl/texts/xml/dblp.xml.50MB.gz | zcat | head -c $LENGTH > data_texts_xml
curl http://pizzachili.dcc.uchile.cl/repcorpus/artificial/fib41.gz | zcat | head -c $LENGTH > data_repcorpus_fib41
curl http://pizzachili.dcc.uchile.cl/repcorpus/artificial/rs.13.gz | zcat | head -c $LENGTH > data_repcorpus_rs13
curl http://pizzachili.dcc.uchile.cl/repcorpus/artificial/tm29.gz | zcat | head -c $LENGTH > data_repcorpus_tm29
curl http://pizzachili.dcc.uchile.cl/repcorpus/pseudo-real/dblp.xml.00001.1.gz | zcat | head -c $LENGTH > data_repcorpus_dblp_xml_1
curl http://pizzachili.dcc.uchile.cl/repcorpus/pseudo-real/dblp.xml.00001.2.gz | zcat | head -c $LENGTH > data_repcorpus_dblp_xml_2
curl http://pizzachili.dcc.uchile.cl/repcorpus/pseudo-real/dblp.xml.0001.1.gz | zcat | head -c $LENGTH > data_repcorpus_dblp_xml_3
curl http://pizzachili.dcc.uchile.cl/repcorpus/pseudo-real/dblp.xml.0001.2.gz | zcat | head -c $LENGTH > data_repcorpus_dblp_xml_4
curl http://pizzachili.dcc.uchile.cl/repcorpus/pseudo-real/dna.001.1.gz | zcat | head -c $LENGTH > data_repcorpus_dna
curl http://pizzachili.dcc.uchile.cl/repcorpus/pseudo-real/english.001.2.gz | zcat | head -c $LENGTH > data_repcorpus_english
curl http://pizzachili.dcc.uchile.cl/repcorpus/pseudo-real/proteins.001.1.gz | zcat | head -c $LENGTH > data_repcorpus_proteins
curl http://pizzachili.dcc.uchile.cl/repcorpus/pseudo-real/sources.001.2.gz | zcat | head -c $LENGTH > data_repcorpus_sources
curl http://pizzachili.dcc.uchile.cl/repcorpus/real/Escherichia_Coli.gz | zcat | head -c $LENGTH > data_repcorpus_e_coli
curl http://pizzachili.dcc.uchile.cl/repcorpus/real/cere.gz | zcat | head -c $LENGTH > data_repcorpus_cere
curl http://pizzachili.dcc.uchile.cl/repcorpus/real/coreutils.gz | zcat | head -c $LENGTH > data_repcorpus_coreutils
curl http://pizzachili.dcc.uchile.cl/repcorpus/real/einstein.de.txt.gz | zcat | head -c $LENGTH > data_repcorpus_einstein_de
curl http://pizzachili.dcc.uchile.cl/repcorpus/real/einstein.en.txt.gz | zcat | head -c $LENGTH > data_repcorpus_einstein_en
curl http://pizzachili.dcc.uchile.cl/repcorpus/real/influenza.gz | zcat | head -c $LENGTH > data_repcorpus_influenza
curl http://pizzachili.dcc.uchile.cl/repcorpus/real/kernel.gz | zcat | head -c $LENGTH > data_repcorpus_kernel
curl http://pizzachili.dcc.uchile.cl/repcorpus/real/para.gz | zcat | head -c $LENGTH > data_repcorpus_para
curl http://pizzachili.dcc.uchile.cl/repcorpus/real/world_leaders.gz | zcat | head -c $LENGTH > data_repcorpus_world_leaders

TEXTS="data_texts_sources data_texts_pitches data_texts_proteins data_texts_dna
data_texts_english data_texts_xml data_repcorpus_fib41 data_repcorpus_rs13
data_repcorpus_tm29 data_repcorpus_dblp_xml_1 data_repcorpus_dblp_xml_2
data_repcorpus_dblp_xml_3 data_repcorpus_dblp_xml_4 data_repcorpus_dna
data_repcorpus_english data_repcorpus_proteins data_repcorpus_sources
data_repcorpus_e_coli data_repcorpus_cere data_repcorpus_coreutils
data_repcorpus_einstein_de data_repcorpus_einstein_en data_repcorpus_influenza
data_repcorpus_kernel data_repcorpus_para data_repcorpus_world_leaders"

for FILENAME in $TEXTS
do
	if [ ! -f $FILENAME ];
	then
		echo "File download was not successful, $FILENAME does not exist. Please retry."
		exit
	fi
	SIZE=$(wc -c < $FILENAME)
	if [ $SIZE -ne $LENGTH ];
	then
		echo "File download was not successful, $FILENAME has wrong size. Please retry."
		exit
	fi
done
