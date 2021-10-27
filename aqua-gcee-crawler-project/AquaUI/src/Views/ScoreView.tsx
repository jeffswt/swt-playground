
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

export { ScoreView };

interface IProps {
    univId: number | undefined;
}

interface IState {}

class ScoreView extends React.Component<IProps, IState> {
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
            <SectionTitle title="投档线" />
            {this._scores(entry)}
            {this._desc(entry)}
            <SectionTiles title="高校排名" entries={this._rankings(entry)} />
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

    private _scores(entry: FacilityEntry) {
        let tmp = [];
        for (let i of entry.baselines)
            tmp.push({
                pv: i.province,
                year: i.year,
                batch: i.batch,
                score: i.score,
            });
        let cols = ['pv', 'year', 'batch', 'score'];
        let titles = {
            pv: '省份',
            year: '年份',
            batch: '科类',
            score: '投档线',
        };
        return <FastTable data={tmp} cols={cols} titles={titles} />
    }
}
