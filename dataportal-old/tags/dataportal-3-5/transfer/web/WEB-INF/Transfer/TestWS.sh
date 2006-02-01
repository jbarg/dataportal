#!/bin/bash

if [ "$1" == ""-h"" ]; then
        echo "Usage: TestWS sessionmanagerUrl sid datatransferURL fromUrl toUrl"
else
        if [ "$1" == ""-sid"" ]; then
                . setEnv.sh
                sm1=$SM
                dt1=$DT
                f1=$fromURL
                t1=$toURL
                echo $t1

        else

                for i in `ls ../libTransfer/*.jar`
                do
                export CLASSPATH=$CLASSPATH:$i
                done

                $JAVA_HOME/bin/java uk.ac.clrc.dataportal.TestTransfer $1 $2 $3 $4 $5
        fi
fi
