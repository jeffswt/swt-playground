
import React from 'react';
import * as FaIcons from '@fortawesome/free-solid-svg-icons'
import { FacilityEntry } from '../Models/DbModels';
import '../Styles/UnivHeader.scss';

export { UnivHeader };

interface IProps {
    entry: FacilityEntry;
}

interface IState {}

class UnivHeader extends React.Component<IProps, IState> {
    props: IProps;

    constructor(props: IProps) {
        super(props);
        this.props = props;
        this.state = {};
    }

    render() {
        let entry = this.props.entry;
        return <div className="univ-header">
            <div className="logo">
                <img src={`/logos/${entry.src_id}.jpg`} />
            </div>
            <div className="info">
                <div className="align-top">
                    <div className="title">{entry.fac_name}</div>
                    <div className="addr">{entry.contact.location}</div>
                </div>
                <div className="buffs">{this._buffs()}</div>
            </div>
        </div>
    }

    private _buffs() {
        let buffs = [];
        let entry = this.props.entry;
        buffs.push(entry.fac_group);
        buffs.push(entry.fac_pub);
        buffs.push(entry.fac_type);
        buffs.push(entry.props.affiliate);
        if (entry.props.is_985)
            buffs.push('985工程');
        if (entry.props.is_211)
            buffs.push('211工程');
        if (entry.props.is_fc)
            buffs.push(entry.props.is_fc);
        return buffs.map((x) => <span className="buff">{x}</span>);
    }
}
