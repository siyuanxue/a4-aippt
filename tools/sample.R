#!/usr/bin/env Rscript

args = commandArgs();
args = args[-(1:5)]
## print(args);

Sample = function(infile, number, outfile, rep, limitPv)
{
    ## print(rep);
    ## print(infile);
    ## print(number);
    ## print(outfile);

    data= read.table(infile, sep = "\t", quote = "", comm = "", as.is = T, encoding = "UTF-8", fill = TRUE);
    if (length(data[1,]) > 1)
    {
        pos = is.na(data[,2]);
        data = data[!pos,];
        
        pos2 = data[,2] >= limitPv;
        data = data[pos2,];
    }
    else
    {
        data = cbind(data, 1);
    }

    print(paste("totalQuery:",length(data[,1])));
    print(sum(is.na(data[,2])));
    print(number);
    sampleRes = sample(data[,1], size = number,replace = rep, prob = data[,2]);
    write.table(sampleRes, outfile, sep = "\t", quote = F, row.names = F, col.names = F, fileEncoding = "UTF-8");
}

if (length(args) < 3) {
    print("Command Error!");
    print("./sample.R inFile 100 outFile --rep=false --limitpv=1")
} else {
    print (paste("InputFile: ",args[1]));
    print (paste("SampleNum: ",args[2]));
    print (paste("OutFile:   ",args[3]));
    replace = FALSE;
    limitPv = 1;
    if (length(args) >= 4)
    {
        splitRes = strsplit(args[4], "=")[[1]];
        ## print(splitRes);
        if (length(splitRes) == 2)
        {
            if (splitRes[1] == '--rep')
            {
                lowRes = tolower(splitRes[2]);
                if (lowRes == "f" || lowRes == "false")
                {
                    replace = FALSE;
                }
                if (lowRes == "t" || lowRes == "true")
                {
                    replace = TRUE;
                }
            }
            else if (splitRes[1] == "--limitpv")
            {
                limitPv = as.numeric(splitRes[2]);
            }
        }
    }

    if (length(args) >= 5)
    {
        splitRes = strsplit(args[5], "=")[[1]];
        ## print(splitRes);
        if (length(splitRes) == 2)
        {
            if (splitRes[1] == '--rep')
            {
                lowRes = tolower(splitRes[2]);
                if (lowRes == "f" || lowRes == "false")
                {
                    replace = FALSE;
                }
                if (lowRes == "t" || lowRes == "true")
                {
                    replace = TRUE;
                }
            }
            else if (splitRes[1] == "--limitpv")
            {
                limitPv = as.numeric(splitRes[2]);
            }
        }
    }

    
    print(paste("Replace: ",replace));
    print(paste("LimitPv: ",limitPv));
    Sample(args[1], args[2], args[3], replace, limitPv);
}
