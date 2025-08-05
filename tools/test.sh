python crawl.py 10.99.0.58:19004 top1k.res new
python crawl.py 10.99.0.58:19000 top1k.res old
# python crawl_old.py 10.99.0.58:11112 sample50k.res out_old5
# python crawl.py 10.99.0.58:11111 sample2k.res out_new5
# python crawl_old.py 10.99.0.58:11112 sample2k.res out_old5
# python crawl.py 10.99.0.58:11111 sample500.res.6 out_new2
# python crawl_old.py 10.99.0.58:11112 sample500.res.6 out_old2
# python crawl.py 10.99.0.58:11111 top100k.res out_new2
# python crawl_old.py 10.99.0.58:11112 top100k.res out_old2
scp new kangping.yinkp@10.99.0.38:/dataAnlysis/dataplatform/asyncModelDiff/Result/
scp old kangping.yinkp@10.99.0.38:/dataAnlysis/dataplatform/asyncModelDiff/Result/