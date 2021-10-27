
import React from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome'
import * as FaIcons from '@fortawesome/free-solid-svg-icons'
import '../Styles/NavPane.scss';
import { TabLocation } from '../Models/TabLocation';

export { NavPane };

interface IProps {
    currentTab: TabLocation;
    onTabChange: (newTab: TabLocation) => void;
}

interface IState {}

class NavPane extends React.Component<IProps, IState> {
    props: IProps;

    constructor(props: IProps) {
        super(props);
        this.props = props;
    }

    render() {
        return <div className="nav-pane">
            <div className="nav-pane-button logo">
                <span>AquaUI</span>
            </div>
            {this._button(TabLocation.univList)}
            {this._button(TabLocation.scEstimation)}
            {this._button(TabLocation.formSub)}
        </div>
    }

    private _button(tab: TabLocation): JSX.Element {
        var caption = '';
        var icon = FaIcons.faAdjust;
        if (tab === TabLocation.univList) {
            caption = '学校列表';
            icon = FaIcons.faListUl;
        } else if (tab === TabLocation.scEstimation) {
            caption = '分数线预测';
            icon = FaIcons.faCalculator;
        } else if (tab === TabLocation.formSub) {
            caption = '填报志愿';
            icon = FaIcons.faPen;
        }
        var className = 'nav-pane-button normal' + (
            this.props.currentTab === tab ? ' active' : '');
        return <div className={className}
                onClick={() => this.props.onTabChange(tab)}>
            <FontAwesomeIcon icon={icon} />
            <span className="caption">{caption}</span>
        </div>
    }
}
