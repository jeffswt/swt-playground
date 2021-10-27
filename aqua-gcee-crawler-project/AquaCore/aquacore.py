
import asyncio
import bs4
from dataclasses import dataclass
import json
import lxml
import os
from playwright.async_api import async_playwright
import requests
import shutil
import time
import tqdm
from typing import Dict, List, Set, Union

from megumin import Megumin
from linq import linq, safedict, SerializableDataclass, serialize


@dataclass
class FacilityBriefing(SerializableDataclass):
    fcid: int      # 院校编号
    name: str      # 院校名称
    province: str  # 所在省份
    is_985: bool   # 985?
    is_211: bool   # 211?
    is_fc: bool    # 是否双一流
    pass


@dataclass
class FacilityProps(SerializableDataclass):      # 基本属性
    affiliate: str        # 隶属于
    is_985: bool          # 985?
    is_211: bool          # 211?
    is_fc: str            # 双一流院校类型
        # A: A类, B: B类, S: 一流学科, '': 无
    fc_ranks: Dict[str, int]
        # A+, A, A-, B+, B, B-, C+, C, C- => 双一流专业数
    founded: int          # 建校年份
    pass


@dataclass
class FacilityAwards(SerializableDataclass):  # 荣誉
    phd_point: int     # 博士点
    ms_point: int      # 硕士点
    academicians: int  # 院士数量
    sp_subjects: int   # 国家重点学科
    sp_labs: int       # 国家重点实验室
    area: float        # 占地面积
    pass


@dataclass
class FacilityRanks(SerializableDataclass):  # 学校排名
    arwu: int         # 上交软科
    qs: int           # QS Rankings
    usn: int          # U.S. News
    times: int        # Times Rankings
    pass


@dataclass
class FacilityCareer(SerializableDataclass):        # 就业情况
    locs: Dict[str, float]   # 去向省市
    types: Dict[str, float]  # 去向单位类别
    corps: Dict[str, int]    # 部分去向企业统计
    rate_abroad: float       # 出国率
    rate_ms: float           # 读研率
    rate_career: float       # 就业率
    pass


@dataclass
class FacilityContact(SerializableDataclass):  # 学校联系方式
    location: str       # 校园地址
    location_dc: str    # 校园所在城市/区县
    location_pv: str    # 校园所在省级行政单位
    emails: List[str]   # 招生办等电邮
    phones: List[str]   # 联系电话
    sites: List[str]    # 官网 URL
    postcode: str       # 邮编
    pass


@dataclass
class FacilityMajorPlan(SerializableDataclass):  # 招生计划
    name: str             # 专业名称
    count: int            # 计划招生人数
    duration: str         # 学制
    tuition: float        # 学费
    requires: str         # 选课要求（选考省份限定）
    pass


@dataclass
class FacilityBaseline(SerializableDataclass):  # 投档线记录
    year: int      # 年份
    batch: str     # 科类 / 批次
    province: str  # 省份
    score: float   # 投档线
    pass


@dataclass
class FacilityEntry(SerializableDataclass):  # 院校信息
    src_id: int               # 数据来源：院校 ID
    fac_name: str             # 院校名称
    fac_type: str             # 院校分类（综合/农业/理工..）
    fac_group: str            # 院校类型（普本/专科..）
    fac_pub: str              # 公办 / 民办
    props: FacilityProps      # 详细属性
    awards: FacilityAwards    # 荣誉称号
    ranks: FacilityRanks      # 排名
    career: FacilityCareer    # 就业
    contact: FacilityContact  # 联络方式
    desc: str                 # 简介
    major_plans: Dict[str, List[FacilityMajorPlan]]
        # 各省份汉字全称 -> 所有年份所有专业招生计划
    baselines: List[FacilityBaseline]
        # 所有查到的学校投档线（按科类，不按专业）
    pass


class AquaCore:
    def __init__(self):
        with open('mappings.json', 'r', encoding='utf-8') as f:
            self._mappings = safedict(json.loads(f.read()))
        return

    def _provinces(self) -> List[str]:
        """ 所有希望爬取的省份汉字名称列表 """
        return ['北京', '山东']

    async def get_all_univs(self) -> List[FacilityBriefing]:
        """ 获取所有院校的编号 """
        results = []
        page = 1
        while True:
            tmp = await self.get_all_univs_paged(page)
            for v in tmp:
                results.append(v)
            if len(tmp) < 30:
                break
            page += 1
        results.sort(key=lambda fb: fb.fcid)
        return results

    async def get_all_univs_paged(self, page: int) -> List[FacilityBriefing]:
        """ 获取所有院校编号的第 page 页 """
        req = Megumin.get(
            f'http://api.eol.cn/gkcx/api/?uri=apidata/api/gk/school/lists'
            f'&page={page}&size=30')
        items = safedict(json.loads(req.decode('utf-8')))['data']['item'](list)
        results = []
        for item in items:
            d = safedict(item)
            results.append(FacilityBriefing(
                fcid=d['school_id'](int),
                name=d['name'](str),
                province=d['province_name'](str),
                is_985=not bool(d['f985'](int) - 1),
                is_211=not bool(d['f211'](int) - 1),
                is_fc=not not d['dual_class_name'](str),
            ))
        results.sort(key=lambda fb: fb.fcid)
        return results

    async def get_data(self, univ_id: int) -> FacilityEntry:
        """ 提供院校 ID，获取有关其的所有数据 """
        # retrieve data
        req = Megumin.get(
            f'http://static-data.eol.cn/www/2.0/school/{univ_id}/info.json')
        jom = json.loads(req.decode('utf-8'))
        data = safedict(jom)['data']
        # parse all
        return FacilityEntry(
            src_id=data['school_id'](int),
            fac_name=data['name'](str),
            fac_type=self._mappings['院校类型'][
                data['type'](str)
            ](str),
            fac_group=self._mappings['院校分类'][
                data['school_type'](str)
            ](str),
            fac_pub=self._mappings['院校办学模式'][
                data['school_nature'](str)
            ](str),
            props=FacilityProps(
                affiliate=data['belong'](str),
                is_985=not bool(data['f985'](int) - 1),
                is_211=not bool(data['f211'](int) - 1),
                is_fc=self._mappings['双一流院校评级'][
                    data['dual_class'](str)
                ](str),
                fc_ranks=linq(data['xueke_rank'](dict).items())\
                    .to_dict(lambda kv: kv[0].upper(), lambda kv: int(kv[1])),
                founded=data['create_date'](int),
            ),
            awards=FacilityAwards(
                phd_point=data['num_doctor'](int),
                ms_point=data['num_master'](int),
                academicians=data['num_academician'](int),
                sp_subjects=data['num_subject'](int),
                sp_labs=data['num_lab'](int),
                area=data['area'](float),
            ),
            ranks=FacilityRanks(
                arwu=data['ruanke_rank'](int),
                qs=data['qs_rank'](int),
                usn=data['us_rank'](int),
                times=data['wsl_rank'](int),  # unconfirmed
            ),
            career=await self.get_career_data(univ_id),
            contact=FacilityContact(
                location=data['address'](str),
                location_dc=self._mappings['县级:id->汉字'][
                    data['county_id'](str)
                ](str),
                location_pv=self._mappings['省级:id->汉字'][
                    data['province_id'](str)
                ](str),
                emails=list(linq([
                    data['email'](str),
                    data['school_email'](str),
                ]).where(lambda x: x).to_set()),
                phones=list(linq([
                    data['phone'](str),
                    data['school_phone'](str),
                ]).map_many(lambda x: x.split(',')) \
                    .map(lambda x: x.strip()) \
                    .where(lambda x: x) \
                    .to_set()),
                sites=list(linq([
                    data['site'](str),
                    data['school_site'](str),
                ]).where(lambda x: x).to_set()),
                postcode=data['postal'](str),
            ),
            desc=data['content'](str).strip(),
            major_plans=await self.get_all_major_plans(univ_id),
            baselines=await self.get_all_baselines(univ_id),
        )

    async def get_career_data(self, univ_id: int) -> FacilityCareer:
        """ 提供院校 ID，获取所有就业相关数据 """
        # retrieve data
        req = Megumin.get(f'http://static-data.eol.cn/www/2.0/'
            f'school/{univ_id}/pc_jobdetail.json')
        jom = json.loads(req.decode('utf-8'))
        data = safedict(jom)['data']
        provinces = data['province']()
        if type(provinces) == dict:
            provinces = list(v for k, v in provinces.items())
        jobrate = {}
        for i, d in data['jobrate'].items():
            for j, v in d.items():
                jobrate[i] = max(jobrate.get(i, 0.0), float(v) / 100.0)
        return FacilityCareer(
            locs=linq(provinces) \
                .map(lambda p: (p['province'], float(p['rate']))) \
                .to_dict(lambda kv: kv[0], lambda kv: kv[1] / 100.0),
            types=linq(data['attr'](dict).items()) \
                .to_dict(lambda kv: kv[0], lambda kv: float(kv[1]) / 100.0),
            corps=linq(data['company'](dict).items()) \
                .to_dict(lambda kv: kv[0], lambda kv: int(kv[1])),
            rate_abroad=jobrate.get('abroad', 0.0),
            rate_ms=jobrate.get('postgraduate', 0.0),
            rate_career=jobrate.get('job', 0.0),
        )

    async def get_all_major_plans(self, univ_id: int
                                  ) -> Dict[str, List[FacilityMajorPlan]]:
        """ 获取院校 ID 的所有专业招生计划 """
        result = {}
        for pv in self._provinces():
            pvid = self._mappings['省级:汉字->id'][pv](int)
            result[pv] = await self.get_major_plans_pv(univ_id, pvid)
        return result

    async def get_major_plans_pv(self, univ_id: int, prov_id: int
                              ) -> List[FacilityMajorPlan]:
        """ 获取院校 ID 在省份 ID 的所有专业招生计划 """
        results = []
        pg = 1
        while True:
            tmp = await self.get_major_plans_pv_paged(univ_id, prov_id, pg)
            for val in tmp:
                results.append(val)
            if len(tmp) < 30:
                break
            pg += 1
        return results

    async def get_major_plans_pv_paged(self, univ_id: int, prov_id: int,
                                       page: int) -> List[FacilityMajorPlan]:
        """ 获取院校 ID 在省份 ID 的专业招生计划的第 [page] 页 """
        url = (f'http://api.eol.cn/gkcx/api/?uri=apidata/api/gkv3/plan/school'
               f'&school_id={univ_id}&local_province_id={prov_id}'
               f'&page={page}&size=30')
        if not Megumin.has(url):
            return []
        req = Megumin.get(url)
        jom = json.loads(req.decode('utf-8'))
        data = safedict(jom)['data']
        result = []
        for _d in data['item'](list):
            d = safedict(_d)
            result.append(FacilityMajorPlan(
                name=d['spname'](str),
                count=d['num'](int),
                duration=d['length'](str),
                tuition=d['tuition'](float),
                requires=d['sg_info'](str),
            ))
        return result

    async def get_all_baselines(self, univ_id: int) -> List[FacilityBaseline]:
        """ 获取学校投档线，不按专业分类 """
        req = Megumin.get(
            f'http://static-data.eol.cn/www/2.0/school/{univ_id}/info.json')
        jom = json.loads(req.decode('utf-8'))
        data = safedict(jom)['data']
        # filter
        results: List[FacilityBaseline] = []
        for pvid, pd in data['pro_type_min'](dict).items():
            for item in pd:
                for typ, score in item['type'].items():
                    results.append(FacilityBaseline(
                        year=item['year'],
                        batch=self._mappings['科类:id->汉字'][typ](str),
                        province=self._mappings['省级:id->汉字'][pvid](str),
                        score=float(score),
                    ))
        results.sort(key=lambda i: (i.province, i.year, i.batch))
        return results

    # async def get_major_scores(self, page, univ_id: int) -> None:
    #     """ 获取专业投档线，但是 webdriver 不好搞 """
    #     # initialize page
    #     await page.goto(f'http://gkcx.eol.cn/school/{univ_id}/provinceline')
    #     # extract recruitment plans
    #     return

    async def pull_image(self, univ_id: int) -> None:
        req = Megumin.get(
            f'http://static-data.eol.cn/upload/logo/{univ_id}.jpg')
        with open(f'./assets/logos/{univ_id}.jpg', 'wb') as f:
            f.write(req)
        return

    async def main(self) -> None:
        # get university list
        univ_list = linq(await self.get_all_univs()) \
            .where(lambda i: '学院' not in i.name or i.is_985 or i.is_211) \
            .to_list()
        # batch get data
        results = []
        for _i, univ in enumerate(univ_list):
            print(f'#{univ.fcid}: {univ.name} ({_i}/{len(univ_list)})')
            data = await self.get_data(univ.fcid)
            results.append(json.dumps(serialize(data), ensure_ascii=False))
        # write to file
        final = '[\n' + ',\n'.join(results) + '\n]\n'
        with open('./assets/megumin-data.json', 'w', encoding='utf-8') as f:
            f.write(final)
        # batch images
        for _i, univ in enumerate(univ_list):
            print(f'#{univ.fcid}: {univ.name} ({_i}/{len(univ_list)})')
            await self.pull_image(univ.fcid)
        return
    pass


asyncio.run(AquaCore().main())
