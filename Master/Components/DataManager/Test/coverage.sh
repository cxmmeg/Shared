#! /bin/bash

# author : arthur li
# date   : 2014.4.20

lcov -z -d ..
lcov -c -i -d .. -b $PWD -o base.info

find bin_rel -name 'ut*' -exec {} \;

lcov -c -d .. -b $PWD -o coverage.info

lcov -a base.info -a coverage.info -o total.info

lcov -r total.info '*Threads/Include*' '*PasswordManager/Include*' '*Global/Include*' '/usr/*' '*opensource-src*' 'moc_*' '*Test/*' '*HimalayaDataContainer*' -o total.info

genhtml -o html total.info

rm -f *.info
