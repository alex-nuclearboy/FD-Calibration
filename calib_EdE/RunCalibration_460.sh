#!/bin/bash
for X in `seq 46050 1 46074`; do
        echo "run #${X}..."
        output="files/calib_EdE_run_${X}"
        if [ -e ${output}.root ]; then
                echo "... is already done."
        else
                RUNCAT=""
                #command find is in cycle because file list may change after each analysis
                zezeze=`find ${RUNS_DATA}|grep ${X}.xz`
                if [ "${zezeze}" != "" ]; then
                        if [ -e ${zezeze} ]; then
                                RUNCAT="xzcat ${zezeze}"
                        fi
                else
                        zezeze=`find ${RUNS_DATA}|grep ${X}.bz2`
                        if [ "${zezeze}" != "" ]; then
                                if [ -e ${zezeze} ]; then
                                        RUNCAT="bzcat ${zezeze}"
                                fi
                        fi
                fi
                if [ "${RUNCAT}" != "" ]; then
                        echo "Run nr ${X}"
                        scriptname="calib_${X}.sh"
                        if [ -e ${scriptname} ]; then
                                echo "...already in process"
                        else
                                echo "... CALIBRATION ..."
                                echo "#!/bin/bash" >> ${scriptname}
                                echo "${RUNCAT}|./main -local -fin cluster: -r ${X} -n ${output} -abort" >> ${scriptname}
                                echo "rm -f $PWD/${scriptname}" >> ${scriptname}
                                chmod u+x ${scriptname}
                                ./${scriptname} &> ${output}.log
                                echo "...done."
                        fi
                fi
        fi
done
