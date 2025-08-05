echo $1
echo "curl http://42.120.173.15/ark_web/topQuery/download/type/tq_hour/real/1/date/$1/hour/00/file/uc_topquery.0 -F user=kangping.yinkp -F pass=Ykp7462265 > $1.query"
curl http://42.120.173.15/ark_web/topQuery/download/type/tq_hour/real/1/date/$1/hour/00/file/uc_topquery.0 -F user=kangping.yinkp -F pass=Ykp7462265 > $1.hour.query
