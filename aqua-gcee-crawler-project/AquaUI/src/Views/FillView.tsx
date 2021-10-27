
import React from 'react';
import * as ReCharts from 'recharts';
import * as FaIcons from '@fortawesome/free-solid-svg-icons'
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome'
import { DbLoader } from '../Data/DbLoader';
import { FacilityEntry } from '../Models/DbModels';
import '../Styles/FillView.scss';
import { UnivHeader } from './UnivHeader';
import { TileEntry, SectionTiles } from './SectionTiles';
import { SectionTitle } from './SectionTitle';
import { FastTable } from './FastTable';
import { PickableTable } from './PickableTable';

export { FillView };

interface IProps {
    univId: number | undefined;
}

interface IState {
    _ticker: number;
}

class FillView extends React.Component<IProps, IState> {
    static chosenMajors: {[key: string]: any}[] = [];

    props: IProps;

    constructor(props: IProps) {
        super(props);
        this.props = props;
        this.state = {
            _ticker: 0,
        };
    }

    render() {
        if (this.props.univId === undefined)
            return this._emptyWidget();
        let entry = this._getEntry();
        return <div className="fill-view">
            <div className="fill-view-top">
                <SectionTitle title="招生计划" />
                {this._majors(entry)}
            </div>
            <div className="fill-view-bottom">
                <SectionTitle title="我的志愿" />
                {this._myFill(entry)}
                <div className="csv-export"
                        onClick={() => this._export()}>
                    导出 CSV 报表
                </div>
            </div>
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
        return <PickableTable data={tmp} cols={cols} titles={titles}
            onClick={(row) => this._addMajor(entry, row)}
            height="low" btn="添加"/>
    }

    private _isEq(entry: FacilityEntry, upd: {[key: string]: any},
        old: {[key: string]: any}): boolean {
        if (upd['pv'] !== old['pv'])
            return false;
        if (entry.fac_name !== old['univ'])
            return false;
        if (upd['major'] !== old['major'])
            return false;
        return true;
    }

    private _addMajor(entry: FacilityEntry, row: {[key: string]: any}) {
        if (FillView.chosenMajors.length >= 60) {
            alert('最多只能填报60个志愿！');
            return;
        }
        for (let i of FillView.chosenMajors)
            if (this._isEq(entry, row, i)) {
                console.log(entry, row, i);
                alert('重复填报志愿！已拒绝操作。');
                return;
            }
        FillView.chosenMajors.push({
            pv: row['pv'],
            univ: entry.fac_name,
            major: row['major'],
            deps: row['deps'],
            duration: row['duration'],
            score: entry.evalScore,
        });
        console.log(FillView.chosenMajors);
        this._tick();
        return;
    }

    private _myFill(entry: FacilityEntry) {
        let cols = ['i', 'pv', 'univ', 'major', 'deps', 'duration', 'score'];
        let tmp = [];
        let cnt = 0;
        for (let i of FillView.chosenMajors)
            tmp.push({
                i: `${++cnt}`,
                pv: i['pv'],
                major: i['major'],
                deps: i['deps'],
                duration: i['duration'],
                count: i['count'],
                tuition: i['tuition'],
            });
        let titles = {
            i: '优先级',
            pv: '省份',
            major: '院校',
            deps: '专业',
            duration: '选考课程',
            count: '学制',
            tuition: '期望投档线',
        };
        return <PickableTable data={tmp} cols={cols}
            titles={titles} onClick={(row) => this._deleteMajor(entry, row)}
            height="medium" btn="移除"/>
    }

    private _deleteMajor(entry: FacilityEntry, row: {[key: string]: any}) {
        let res = [];
        for (let i of FillView.chosenMajors)
            if (!this._isEq(entry, row, i))
                res.push(i);
        FillView.chosenMajors = res;
        this._tick();
    }

    private _tick() {
        this.setState({
            _ticker: this.state._ticker + 1,
        });
    }

    private _export() {
        // export to csv
        let lines = ['优先级,省份,院校,专业,选考课程,学制,期望投档线'];
        let cnt = 0;
        for (let i of FillView.chosenMajors)
            lines.push([
                `${++cnt}`,
                i['pv'],
                i['major'],
                i['deps'],
                i['duration'],
                i['count'],
                i['tuition'],
            ].join(','));
        let content = lines.join('\n') + '\n';
        var bb = new Blob([content], { type: 'application/csv' });
        var a = document.createElement('a');
        a.download = 'exportSheet.csv';
        a.href = window.URL.createObjectURL(bb);
        a.click();
    }
}
