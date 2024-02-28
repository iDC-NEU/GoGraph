files=("$1")

for filename in ${files[@]}
do
    ./pretreatment.sh ${filename}
    ./gograph.sh ${filename}
    ./partvertex.sh ${filename}
    ./ProcessBigdegreePoint.sh ${filename}
    ./ProcessIsolatedPoint.sh ${filename}
    ./edgemap.sh ${filename}
done
