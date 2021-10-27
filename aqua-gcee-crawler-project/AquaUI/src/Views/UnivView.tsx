
import React from 'react';
import * as ReCharts from 'recharts';
import * as FaIcons from '@fortawesome/free-solid-svg-icons'
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome'
import { DbLoader } from '../Data/DbLoader';
import { FacilityEntry } from '../Models/DbModels';
import '../Styles/UnivView.scss';
import { UnivHeader } from './UnivHeader';
import { TileEntry, SectionTiles } from './SectionTiles';
import { SectionTitle } from './SectionTitle';
import { FastTable } from './FastTable';

export { UnivView };

interface IProps {
    univId: number | undefined;
}

interface IState {}

class UnivView extends React.Component<IProps, IState> {
    props: IProps;

    constructor(props: IProps) {
        super(props);
        this.props = props;
        this.state = {};
    }

    render() {
        if (this.props.univId === undefined)
            return this._emptyWidget();
        let entry = this._getEntry();
        return <div className="right-panel univ-view">
            <UnivHeader entry={entry}/>
            {this._desc(entry)}
            <SectionTiles title="高校排名" entries={this._rankings(entry)} />
            <SectionTiles title="学术概况" entries={this._academia(entry)} />
            <SectionTiles title="双一流学科" entries={this._fc(entry)} />
            {this._info(entry)}
            {this._career(entry)}
            <SectionTitle title="招生计划" />
            {this._majors(entry)}
            <div style={{height: '5em'}} />
        </div>
    }

    private _emptyWidget() {
        return <div className="right-panel univ-view empty">
            请选择学校
        </div>
    }

    private _getEntry(): FacilityEntry {
        let list = DbLoader.getListSync();
        for (let entry of list)
            if (entry.src_id === this.props.univId)
                return entry;
        return list[0];
    }

    private _desc(entry: FacilityEntry) {
        return <>
            <SectionTitle title="学校简介" />
            <div className="desc-text">{entry.desc}...</div>
        </>
    }

    private _info(entry: FacilityEntry) {
        return <>
            <SectionTitle title="概要信息" />
            {this._infoBlock(FaIcons.faMapMarked, '地址', [entry.contact.location])}
            {this._infoBlock(FaIcons.faMailBulk, '邮箱', entry.contact.emails)}
            {this._infoBlock(FaIcons.faPhone, '电话', entry.contact.phones)}
            {this._infoBlock(FaIcons.faNetworkWired, '官网', entry.contact.sites)}
            {this._infoBlock(FaIcons.faPassport, '邮编', [entry.contact.postcode])}
        </>
    }

    private _infoBlock(icon: FaIcons.IconDefinition, title: string,
        entries: (string | undefined)[]) {
        let disp: string[] = [];
        for (let i of entries)
            if (i !== undefined && i)
                disp.push(i)
        if (disp.length === 0)
            return <></>
        return <div className="univ-info-block">
            <div className="title">
                <FontAwesomeIcon icon={icon} />
                <span className="text">{title}</span>
            </div>
            <div className="items">
                {disp.map((ent) => <div className="info-item">{ent}</div>)}
            </div>
        </div>
    }

    private _rankings(entry: FacilityEntry): TileEntry[] {
        let rankit = (x: number) => x === undefined ? undefined
            : x === 0 ? undefined : `${x}`;
        return [
            new TileEntry('上交软科', rankit(entry.ranks.arwu)),
            new TileEntry('QS Rankings', rankit(entry.ranks.qs)),
            new TileEntry('泰晤士', rankit(entry.ranks.times)),
            new TileEntry('U.S. News', rankit(entry.ranks.usn)),
            new TileEntry('投档线', rankit(entry.evalScore)),
        ];
    }

    private _academia(entry: FacilityEntry): TileEntry[] {
        let countit = (x: number) => x === undefined ? undefined
            : x === null ? undefined
            : x === 0 ? undefined : `${x}`;
        return [
            new TileEntry('博士点', countit(entry.awards.phd_point)),
            new TileEntry('硕士点', countit(entry.awards.ms_point)),
            new TileEntry('院士数量', countit(entry.awards.academicians)),
            new TileEntry('重点实验室', countit(entry.awards.sp_labs)),
            new TileEntry('国家重点学科', countit(entry.awards.sp_subjects)),
        ];
    }

    private _fc(entry: FacilityEntry): TileEntry[] {
        let res: TileEntry[] = [];
        let fc = entry.props.fc_ranks;
        for (let k of Object.keys(fc))
            res.push(new TileEntry(`${k} 类学科`, fc[k]));
        return res;
    }

    private _career(entry: FacilityEntry) {
        let locs: {name: string, data: number}[] = [];
        let typs: {name: string, data: number}[] = [];
        let corps: {name: string, data: number}[] = [];
        let stats: {name: string, data: number}[] = [];
        for (let k of Object.keys(entry.career.locs))
            locs.push({name: k, data: entry.career.locs[k] * 100.0});
        for (let k of Object.keys(entry.career.types))
            typs.push({name: k, data: entry.career.types[k] * 100.0});
        for (let k of Object.keys(entry.career.corps))
            corps.push({name: k, data: entry.career.corps[k]});
        stats.push({name: '出国率', data: entry.career.rate_abroad * 100.0});
        stats.push({name: '读研率', data: entry.career.rate_ms * 100.0});
        stats.push({name: '就业率', data: entry.career.rate_career * 100.0});
        let width = 350;
        let height = 300;
        return <>
            <SectionTitle title="就业情况" />
            <div className="univ-careers">
                {this._chart(locs, '#7986cb', '分区域就业率')}
                {this._chart(typs, '#1e88e5', '就业类型')}
                {this._chart(corps, '#0288d1', '去向企业')}
                {this._chart(stats, '#5c6bc0', '宏观统计')}
            </div>
        </>
    }

    private _chart(data: {name: string, data: number}[], color: string,
        title: string) {
        return <div className="chart">
            <ReCharts.BarChart width={350} height={300} data={data}>
                <ReCharts.CartesianGrid strokeDasharray="1 1" />
                <ReCharts.XAxis dataKey="name" />
                <ReCharts.YAxis />
                <ReCharts.Tooltip />
                <ReCharts.Legend />
                <ReCharts.Bar dataKey="data" fill={color}
                    name={title} />
            </ReCharts.BarChart>
        </div>
    }

    private _majors(entry: FacilityEntry) {
        let tmp = [];
        for (let pv of Object.keys(entry.major_plans))
            for (let i of entry.major_plans[pv])
                tmp.push({
                    pv: pv,
                    major: i.name,
                    deps: i.requires,
                    duration: i.duration,
                    count: i.count,
                    tuition: i.tuition,
                });
        let cols = ['pv', 'major', 'deps', 'duration', 'count', 'tuition'];
        let titles = {
            pv: '省份',
            major: '专业/大类',
            deps: '选考课程',
            duration: '学制',
            count: '计划人数',
            tuition: '学费',
        };
        return <FastTable data={tmp} cols={cols} titles={titles} />
    }
}
