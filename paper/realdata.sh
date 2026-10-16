#!/bin/bash

ascp_download() {
	local source_file=$1
	ascli server download \
		"${source_file}" \
		--log-level=debug \
		--to-folder=${HOME}/sdb1/lwfa/
}

srf2fastq() {
	srf2fastq \
		-s ${HOME}/sdb1/lwfa/ERR091574 \
		${HOME}/sdb1/lwfa/120110_HSQ1009_0086_AD0KU2ACXX-8.srf
}

fast5_to_pod5() {
	pod5 convert fast5 \
		${HOME}/sdb1/lwfa/BOWDEN03_20161104_FNFAB42715_MN16561_WTON000140_NA12878.fast5/*.fast5 \
		--output ${HOME}/sdb1/lwfa/pod5s/
}

map_short_reads() {
	bowtie2 \
		-p 16 \
		-x ${HOME}/.local/share/genomes/hs1/index/bowtie2/hs1 \
		-1 ${HOME}/sdb1/lwfa/ERR091574_1.fastq.gz \
		-2 ${HOME}/sdb1/lwfa/ERR091574_2.fastq.gz |
		samtools view -b - \
			-o ${HOME}/sdb1/lwfa/ERR091574.bam
}

# ascp_download era-fasp@fasp.sra.ebi.ac.uk:/vol1/run/ERR091/ERR091574/120110_HSQ1009_0086_AD0KU2ACXX-8.srf
# ascp_download vol1/run/ERR327/ERR3278877/BOWDEN03_20161104_FNFAB42715_MN16561_WTON000140_NA12878.fast5.tgz
