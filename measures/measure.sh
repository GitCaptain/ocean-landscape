
set -u

sizes="500 1000 10000 100000 1000000"
years="1000 10000 100000 1000000"
mor_cnt="0 1 2"
basin_cnt="0 1 2 3"
margin_cnt="0 1 2"

for sz in ${sizes}; do
    for y in ${years}; do
        for mor in ${mor_cnt}; do
            for bsn in ${basin_cnt}; do
                for mrg in ${margin_cnt}; do
                    output="results/measures_${sz}_${y}_${mor}_${bsn}_${mrg}"
                    logs="logs/logs_${sz}_${y}_${mor}_${bsn}_${mrg}"
                    echo "current setup: output=${output}"
                    echo "size=${sz}, years=${y}, mor=${mor}, basin=${bsn}, margin=${mrg}"
                    taskset -c 5 ./landcape_whole  \
                        --sizex="${sz}" \
                        --sizey="${sz}" \
                        --years="${y}" \
                        --mor-cnt="${mor}" \
                        --basin-cnt="${bsn}" \
                        --margin-cnt="${mrg}" \
                        --output="${output}" > "${logs}"
                done
            done
        done
    done
done
