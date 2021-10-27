
export { FacilityBriefing, FacilityProps, FacilityAwards, FacilityRanks,
         FacilityCareer, FacilityContact, FacilityMajorPlan, FacilityBaseline,
         FacilityEntry };

class FacilityBriefing {  // 简要信息
    fcid: number;      // 院校编号
    name: string;      // 院校名称
    province: string;  // 所在省份
    is_985: boolean;   // 985?
    is_211: boolean;   // 211?
    is_fc: boolean;    // 是否双一流

    constructor(fcid: number, name: string, province: string, is_985: boolean,
        is_211: boolean, is_fc: boolean) {
        this.fcid = fcid;
        this.name = name;
        this.province = province;
        this.is_985 = is_985;
        this.is_211 = is_211;
        this.is_fc = is_fc;
    }
}

class FacilityProps {  // 基本属性
    affiliate: string;        // 隶属于
    is_985: boolean;          // 985?
    is_211: boolean;          // 211?
    is_fc: string;            // 双一流院校类型
        // A: A类, B: B类, S: 一流学科, '': 无
    fc_ranks: {[key: string]: number};
        // A+, A, A-, B+, B, B-, C+, C, C- => 双一流专业数
    founded: number;          // 建校年份

    constructor(affiliate: string, is_985: boolean, is_211: boolean,
        is_fc: string, fc_ranks: {[key: string]: number}, founded: number) {
        this.affiliate = affiliate;
        this.is_985 = is_985;
        this.is_211 = is_211;
        this.is_fc = is_fc;
        this.fc_ranks = fc_ranks;
        this.founded = founded;
    }
}

class FacilityAwards {  // 荣誉
    phd_point: number;     // 博士点
    ms_point: number;      // 硕士点
    academicians: number;  // 院士数量
    sp_subjects: number;   // 国家重点学科
    sp_labs: number;       // 国家重点实验室
    area: number;          // 占地面积

    constructor(phd_point: number, ms_point: number, academicians: number,
        sp_subjects: number, sp_labs: number, area: number) {
        this.phd_point = phd_point;
        this.ms_point = ms_point;
        this.academicians = academicians;
        this.sp_subjects = sp_subjects;
        this.sp_labs = sp_labs;
        this.area = area;
    }
}

class FacilityRanks {  // 学校排名
    arwu: number;   // 上交软科
    qs: number;     // QS Rankings
    usn: number;    // U.S. News
    times: number;  // Times Rankings

    constructor(arwu: number, qs: number, usn: number, times: number) {
        this.arwu = arwu;
        this.qs = qs;
        this.usn = usn;
        this.times = times;
    }
}

class FacilityCareer {  // 就业情况
    locs: {[key: string]: number};   // 去向省市
    types: {[key: string]: number};  // 去向单位类别
    corps: {[key: string]: number};  // 部分去向企业统计
    rate_abroad: number;             // 出国率
    rate_ms: number;                 // 读研率
    rate_career: number;             // 就业率

    constructor(locs: {[key: string]: number}, types: {[key: string]: number},
        corps: {[key: string]: number}, rate_abroad: number, rate_ms: number,
        rate_career: number) {
        this.locs = locs;
        this.types = types;
        this.corps = corps;
        this.rate_abroad = rate_abroad;
        this.rate_ms = rate_ms;
        this.rate_career = rate_career;
    }
}

class FacilityContact {  // 学校联系方式
    location: string;      // 校园地址
    location_dc: string;   // 校园所在城市/区县
    location_pv: string;   // 校园所在省级行政单位
    emails: string[];      // 招生办等电邮
    phones: string[];      // 联系电话
    sites: string[];       // 官网 URL
    postcode: string;      // 邮编

    constructor(location: string, location_dc: string, location_pv: string,
        emails: string[], phones: string[], sites: string[],
        postcode: string) {
        this.location = location;
        this.location_dc = location_dc;
        this.location_pv = location_pv;
        this.emails = emails;
        this.phones = phones;
        this.sites = sites;
        this.postcode = postcode;
    }
}

class FacilityMajorPlan {  // 招生计划
    name: string;      // 专业名称
    count: number;     // 计划招生人数
    duration: string;  // 学制
    tuition: number;   // 学费
    requires: string;  // 选课要求（选考省份限定）

    constructor(name: string, count: number, duration: string, tuition: number,
        requires: string) {
        this.name = name;
        this.count = count;
        this.duration = duration;
        this.tuition = tuition;
        this.requires = requires;
    }
}

class FacilityBaseline {  // 投档线记录
    year: number;      // 年份
    batch: string;     // 科类 / 批次
    province: string;  // 省份
    score: number;     // 投档线

    constructor(year: number, batch: string, province: string, score: number) {
        this.year = year;
        this.batch = batch;
        this.province = province;
        this.score = score;
    }
}

class FacilityEntry {  // 院校信息
    src_id: number;            // 数据来源：院校 ID
    fac_name: string;          // 院校名称
    fac_type: string;          // 院校分类（综合/农业/理工..）
    fac_group: string;         // 院校类型（普本/专科..）
    fac_pub: string;           // 公办 / 民办
    props: FacilityProps;      // 详细属性
    awards: FacilityAwards;    // 荣誉称号
    ranks: FacilityRanks;      // 排名
    career: FacilityCareer;    // 就业
    contact: FacilityContact;  // 联络方式
    desc: string;              // 简介
    major_plans: {[key: string]: FacilityMajorPlan[]};
        // 各省份汉字全称 -> 所有年份所有专业招生计划
    baselines: FacilityBaseline[];
        // 所有查到的学校投档线（按科类，不按专业）
    evalScore: number;         // 估算排序用分数

    constructor(src_id: number, fac_name: string, fac_type: string,
        fac_group: string, fac_pub: string, props: FacilityProps,
        awards: FacilityAwards, ranks: FacilityRanks, career: FacilityCareer,
        contact: FacilityContact, desc: string,
        major_plans: {[key: string]: FacilityMajorPlan[]},
        baselines: FacilityBaseline[]) {
        this.src_id = src_id;
        this.fac_name = fac_name;
        this.fac_type = fac_type;
        this.fac_group = fac_group;
        this.fac_pub = fac_pub;
        this.props = props;
        this.awards = awards;
        this.ranks = ranks;
        this.career = career;
        this.contact = contact;
        this.desc = desc;
        this.major_plans = major_plans;
        this.baselines = baselines;
        this.evalScore = -1;
    }
}
