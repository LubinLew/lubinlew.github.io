#!/usr/bin/python3
# -*- coding: utf-8 -*-

import os
import re
import datetime
import traceback
import geoip2.database
from elasticsearch import Elasticsearch
from elasticsearch.connection import create_ssl_context

def get_es_time(tm):
    timestr = "%d-%02d-%02dT%02d:%02d:00+08:00" % (tm.year, tm.month, tm.day, tm.hour, tm.minute)
    return timestr

# navsec-vpn-2021.01.21
def get_es_index(tm):
    tm = tm - datetime.timedelta(hours=8)
    timestr = "%d.%02d.%02d" % (tm.year, tm.month, tm.day)
    return "navsec-vpn-" + timestr


def serarch_vpn_login():

    es = Elasticsearch(
        ['10.130.12.131'],
        http_auth=('elastic', 'passwrod'),
        scheme='http',
        port='9200'
    )

    localtime  = datetime.datetime.now()
    es_index   = get_es_index(localtime)
    start_time = get_es_time(localtime - datetime.timedelta(minutes=6))
    end_time   = get_es_time(localtime - datetime.timedelta(minutes=1))

    print("[VPN-CHECK][%s - %s]" % (start_time, end_time))

    try:
        query = es.search(
            index=es_index, # 索引名
            size = 100,
            body={          # 请求体
                "query": {
                    "range": {
                        "@timestamp": { # 时间范围
                            'gte': start_time,
                            'lte': end_time
                        }
                    }
                }
            }
        )
    except:
        print(traceback.format_exc())
        return


    vpnlogin = query['hits']['hits']

    reobj1 = re.compile(r"A new TCP tunnel has been established successfully", re.IGNORECASE)
    reobj2 = re.compile(r"user=([^\s]+).+src=([\d\.]+).+clientip\(([\d\.]+)\)", re.IGNORECASE)
    reader = geoip2.database.Reader('/opt/geoip2/GeoLite2-City.mmdb')

    for v in vpnlogin:
        vv = v["_source"]
        prompt = ""
        if 'msg' not in vv.keys():
            continue

        msg = vv['msg']
        if msg != '-':
            result = reobj1.search(msg) # 成功登录消息筛选
            if result: # 登录成功
                searchObj = reobj2.search(vv['message'])
                if searchObj:
                    #print() # user
                    response = reader.city(searchObj.group(2))
                    if response:
                        try:
                            city = response.city.names['zh-CN']
                        except:
                            city = '-'
                        prompt = "  [用户:%s]从[%s:%s(%s)]登录" % (searchObj.group(1), response.country.names['zh-CN'], city, searchObj.group(2))
                        if response.country.names['zh-CN'] != u"中国":
                            with open('/tmp/vpn.txt', encoding="utf-8", mode="w") as out_f:
                                out_f.write('[VPN登录]\n' + prompt)
                            os.system("/opt/waf-tools/alert/vpn_alert.sh >> /var/log/vpn-login.log 2>&1")
                        else:
                            print(prompt)
                    else:
                        print("  [用户:%s]从[%s]登录" % (searchObj.group(1), searchObj.group(2)))



if __name__ == '__main__':
    serarch_vpn_login()