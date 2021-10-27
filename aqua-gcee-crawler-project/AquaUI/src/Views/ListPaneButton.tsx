
import React from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome'
import * as FaIcons from '@fortawesome/free-solid-svg-icons'
import { FacilityEntry } from '../Models/DbModels';
import '../Styles/ListPane.scss';

export { ListPaneButton };

interface IProps {
    entry: FacilityEntry;
    icon: FaIcons.IconDefinition;
    active: boolean;  // if is pressed
    onClick: () => void;
}

interface IState {}

class ListPaneButton extends React.Component<IProps, IState> {
    props: IProps;

    constructor(props: IProps) {
        super(props);
        this.props = props;
    }

    render() {
        let entry = this.props.entry;
        let buffs = [];
        if (entry.props.is_985 === true)
            buffs.push(<span key="985">985</span>);
        if (entry.props.is_211 === true)
            buffs.push(<span key="211">211</span>);
        if (entry.props.is_fc)
            buffs.push(<span key="fc">双一流</span>);
        if (entry.props.affiliate.endsWith('部')) {
            let aff = entry.props.affiliate;
            if (aff === '工业和信息化部')
                aff = '工信部';
            buffs.push(<span key="aff">{aff}</span>);
        }
        if (entry.evalScore > 0)
            buffs.push(<span key="sc">~{entry.evalScore}</span>);
        let className = 'list-pane-button' + (
            this.props.active ? ' active' : '');
        return <div className={className}
                onClick={() => this.props.onClick()}>
            <div className="align-left">
                <div className="logo">
                    <img src={`/logos/${entry.src_id}.jpg`} />
                </div>
                <div className="desc">
                    <div className="title">{entry.fac_name}</div>
                    {buffs.length > 0 ?
                        <div className="buffs">{buffs}</div>
                        : <></>}
                </div>
            </div>
            <div className="icon">
                <FontAwesomeIcon icon={this.props.icon} />
            </div>
        </div>
    }
}
