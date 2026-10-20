#!/bin/bash

prefetch ERR091574 \
	--max-size 100GB \
	--output-directory ${HOME}/sdb1/lwfa/

vdb-validate ${HOME}/sdb1/lwfa/ERR091574/ERR091574.sra

fasterq-dump ${HOME}/sdb1/lwfa/ERR091574/ERR091574.sra \
	--outdir ${HOME}/sdb1/lwfa/ERR091574/

bowtie2 \
	-p 16 \
	-x ${HOME}/.local/share/genomes/hs1/index/bowtie2/hs1 \
	-1 ${HOME}/sdb1/lwfa/ERR091574/ERR091574_1.fastq.gz \
	-2 ${HOME}/sdb1/lwfa/ERR091574/ERR091574_2.fastq.gz |
	samtools view -b - \
		-o ${HOME}/sdb1/lwfa/ERR091574/ERR091574.bam

wget -c \
	https://sra-pub-src-1.s3.amazonaws.com/ERR3278877/BOWDEN03_20161104_FNFAB42715_MN16561_WTON000140_NA12878.fast5.tgz.2 \
	-P ${HOME}/sdb1/lwfa/ERR3278877

tar \
	-zxf ${HOME}/sdb1/lwfa/ERR3278877/BOWDEN03_20161104_FNFAB42715_MN16561_WTON000140_NA12878.fast5.tgz.2 \
	--directory ${HOME}/sdb1/lwfa/ERR3278877

pod5 convert fast5 \
	${HOME}/sdb1/lwfa/BOWDEN03_20161104_FNFAB42715_MN16561_WTON000140_NA12878.fast5/*.fast5 \
	--output ${HOME}/sdb1/lwfa/pod5s/
