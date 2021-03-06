#!/bin/bash

workspace=$(pwd)
parserFolder=${workspace}

app=Lib/parser

# ./a.out < ../Tests/in/*.in > ../Tests/res/*.res


testFolder=${workspace}/Tests
inFolder=${testFolder}/in
outFolder=${testFolder}/out
resultFolder=${testFolder}/res

inExt=\.in
outExt=\.out
llExt=\.ll

totalTests=`ls -1q ${inFolder} | wc -l`

make -C Lib all

# Test Run
for filePath in ${inFolder}/*.in; do
#    dos2unix ${filePath}

# get the file name without extension
    fileName=`basename ${filePath} | cut -d . -f 1`
    outPath=${outFolder}/${fileName}${llExt}

# echo ${filePath}
    echo -e "Running ${fileName}"
    ${app} < ${filePath} > ${outFolder}/${fileName}${llExt}
    lli ${outPath} > ${outFolder}/${fileName}${outExt}
done

# Diff
for filePath in ${outFolder}/*.out; do
#    dos2unix ${filePath}

# get the file name without extension
    fileName=`basename ${filePath} | cut -d . -f 1`

    result=`diff ${filePath} ${resultFolder}/${fileName}${outExt}`
    if [[ "${result}" != "" ]]; then
        # Print result
        # echo -e "\e[35m${result}"
        echo -e "\e[91mTest ${fileName} FAILED"
    fi
done