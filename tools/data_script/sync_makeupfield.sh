#!/bin/bash

filename='application_5885bde0de97e506a57488be8c913e332db7.data'
tmpname='makeupfield_filtered'

wget "https://quark-cdn.alibaba-inc.com/blm/jyapi-483/proxy?path=/jy-online/online/${filename}" -O ./${filename} -o /dev/null

new_md5sum=$(md5sum ./${filename} | awk '{print $1}')
old_md5sum=$(md5sum ./${tmpname} | awk '{print $1}')

if [ "$old_md5sum" != "$new_md5sum" ]
then
    echo "Update"
    mv ./${filename} ./${tmpname}
    python makeupfield_parse.py ./${tmpname} ../data/a4_agg_web_data_new/${tmpname}
else
    echo "Nodiff"
fi
