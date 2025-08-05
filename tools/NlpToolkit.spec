#build command:
#rpmbuild --buildroot=/some_where -bb rpm.spec   --define "_build_id_links none" --define "debug_package %{nil}"

%define debug_package %{nil}

Name        : NlpToolkit-7u
Version     : 2.0.0
Release     : 1
Group       : Applications/Text
License     : Share
Vendor      : Machine Translation Aliyun
Summary     : from NlpToolkit-7u and update so name
Prefix: /usr

%description

%files
/usr/local/include/NlpToolkit
/usr/local/include/NlpToolkit/DictInterface.h
/usr/local/include/NlpToolkit/DictVisitor.h
/usr/local/include/NlpToolkit/DoubleTrieDict.h
/usr/local/include/NlpToolkit/DoubleTrieVisitor.h
/usr/local/include/NlpToolkit/MMap.h
/usr/local/include/NlpToolkit/Serialization.h
/usr/local/include/NlpToolkit/SimpleHeader.h
/usr/local/include/NlpToolkit/Trie.h
/usr/local/include/NlpToolkit/code_detector.h
/usr/local/include/NlpToolkit/detokenizer.h
/usr/local/include/NlpToolkit/encode_converter.h
/usr/local/include/NlpToolkit/encode_type.h
/usr/local/include/NlpToolkit/general_mmap.h
/usr/local/include/NlpToolkit/language_detector.h
/usr/local/include/NlpToolkit/language_type.h
/usr/local/include/NlpToolkit/normalizer.h
/usr/local/include/NlpToolkit/sentence_breaker.h
/usr/local/include/NlpToolkit/toolkit_define.h
/usr/local/include/NlpToolkit/toolkit_error_type.h
/usr/local/include/NlpToolkit/toolkit_utility.h
/usr/local/lib64/libNlpToolkit.a
/usr/local/lib/libNlpToolkit.a.1.0.0
/usr/local/lib64/libNlpToolkit.so
/usr/local/lib/libNlpToolkit.so.1.0.0
/usr/local/libdata/NlpToolkit
/usr/local/libdata/NlpToolkit/conf
/usr/local/libdata/NlpToolkit/conf/code_detector_config.conf
/usr/local/libdata/NlpToolkit/conf/language_detector.conf
/usr/local/libdata/NlpToolkit/conf/sentence_breaker.conf
/usr/local/libdata/NlpToolkit/data
/usr/local/libdata/NlpToolkit/data/language-detector-me.model
/usr/local/libdata/NlpToolkit/data/sentence-breaker-eng-dict.txt
/usr/local/libdata/NlpToolkit/data/sentence-breaker-me.model
/usr/local/libdata/NlpToolkit/example
/usr/local/libdata/NlpToolkit/example/Makefile
/usr/local/libdata/NlpToolkit/example/src
/usr/local/libdata/NlpToolkit/example/src/example_code_detector.cpp
/usr/local/libdata/NlpToolkit/example/src/example_code_detector_input_file.cpp
/usr/local/libdata/NlpToolkit/example/src/example_encode_convert.cpp
/usr/local/libdata/NlpToolkit/example/src/example_language_detector.cpp
/usr/local/libdata/NlpToolkit/example/src/example_normalizer.cpp
/usr/local/libdata/NlpToolkit/example/src/example_sentence_breaker.cpp

