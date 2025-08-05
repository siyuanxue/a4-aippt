#! /usr/bin/env python
# -*- coding:utf-8 -*-

from lxml import etree

def __generate_child_xpath(tag, *attrs):
    if tag is None or len(tag) == 0:
        return None
    xpath = "%s" % (tag)
    if len(attrs) == 0:
        return xpath
    xpath = xpath + '['
    for key, value in attrs.items():
        if len(key) == 0 or value == None:
            return None
        if xpath[-1] != '[':
            xpath = xpath + ' and '
        xpath = xpath + ("@%s='%s'" % (key, value))
    xpath = xpath + ']'
    return xpath

def get_all_child_elements(parent):
    return list(parent)

def get_child_elements(parent, tag, **attrs):
    if parent is None:
        return None
    xpath = __generate_child_xpath(tag, **attrs)
    if not xpath:
        return []
    return parent.xpath(xpath)

def get_uniq_child_element(parent, tag, **attrs):
    elems = get_child_elements(parent, tag, **attrs)
    if elems is None or len(elems) != 1:
        return None
    return elems[0]

def get_attribute(node, attr_name):
    return node.get(attr_name)

def get_text_value(node):
    return node.text

if __name__ == "__main__":
    content = open('testdata/test_mylxml.xml').read()
    root = etree.XML(content)

    print len(get_all_child_elements(root))
    elems = get_child_elements(root, "larmmi")
    print len(elems)
    elems = get_child_elements(root, "node")
    print len(elems)
    elems = get_child_elements(root, "vivian")
    print len(elems)

    elem = get_uniq_child_element(root, "larmmi")
    print get_attribute(elem, "a")
    print get_attribute(elem, "b")
    print get_text_value(elem)
