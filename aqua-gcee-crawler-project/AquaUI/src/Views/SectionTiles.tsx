
import React from 'react';
import '../Styles/SectionTiles.scss';
import { SectionTitle } from './SectionTitle';

export { TileEntry, SectionTiles };

class TileEntry {
    key: string;
    value?: string;

    constructor(key: string, value: string | number | undefined) {
        this.key = key;
        if (value === undefined)
            this.value = undefined
        else
            this.value = value.toString();
    }
}

interface IProps {
    title: string;
    entries: TileEntry[];
}

interface IState {}

class SectionTiles extends React.Component<IProps, IState> {
    props: IProps;

    constructor(props: IProps) {
        super(props);
        this.props = props;
        this.state = {};
    }

    render() {
        let entries = [];
        for (let i of this.props.entries)
            if (i.value !== undefined)
                entries.push(new TileEntry(i.key, i.value));
        if (entries.length === 0)
            return <></>
        // there's really something to render
        return <>
            <SectionTitle title={this.props.title} />
            <div className="section-tiles">
                {entries.map((entry) => <div className="tile">
                    <div className="value">{entry.value}</div>
                    <div className="key">{entry.key}</div>
                </div>)}
            </div>
        </>
    }
}
