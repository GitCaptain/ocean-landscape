
set -u

function measure_whole () {

    binary_name="${1}"
    sizes="500 1000 5000"
    years="1000 5000 10000 15000"
    mor_cnt="1"
    basin_cnt="2"
    margin_cnt="1"

    for sz in ${sizes}; do
        for y in ${years}; do
            for mor in ${mor_cnt}; do
                for bsn in ${basin_cnt}; do
                    for mrg in ${margin_cnt}; do
                        output="results/measures_${sz}_${y}_${mor}_${bsn}_${mrg}_whole"
                        logs="logs/logs_${sz}_${y}_${mor}_${bsn}_${mrg}"
                        echo "current setup: output=${output}"
                        echo "size=${sz}, years=${y}, mor=${mor}, basin=${bsn}, margin=${mrg}"
                        taskset -c 5 ./${binary_name}  \
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
}

function measure_units() {
    binary_name="${1}"
    sizes="500 1000 5000"
    years="1000 5000 10000 15000"

    for sz in ${sizes}; do
        for y in ${years}; do
            output="_${sz}_${y}"
            logs="logs/logs_${sz}_${y}_units"
            echo "current setup: output=${output}, size=${sz}, years=${y}"

            taskset -c 5 ./${binary_name}  \
                --sizex="${sz}" \
                --sizey="${sz}" \
                --years="${y}" \
                --output="${output}" > "${logs}"
        done
    done

}

# measure_whole "whole_measure"
measure_units "units_measure"
