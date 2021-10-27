
import React from 'react';
import { TabLocation } from '../Models/TabLocation';
import { ListPane } from './ListPane';
import { UnivView } from './UnivView';
import { NavPane } from './NavPane';
import '../Styles/App.scss';
import { ScoreView } from './ScoreView';
import { FillView } from './FillView';

export { App };

interface IProps {}

interface IState {
    currentTab: TabLocation;
    currentUnivId?: number;
}

class App extends React.Component<IProps, IState> {
    props: IProps;

    constructor(props: IProps) {
        super(props);
        this.props = props;
        this.state = {
            currentTab: TabLocation.univList,
            currentUnivId: undefined,
        };
    }

    render() {
        let curTab = this.state.currentTab;
        return <div className="app-frame">
            <NavPane currentTab={this.state.currentTab}
                    onTabChange={(x) => this._onTabChange(x)}/>
            <div className="app-content">
                <ListPane onClick={(x) => this._onUnivChange(x)}/>
                {curTab === TabLocation.univList ?
                    <UnivView univId={this.state.currentUnivId} />
                : curTab === TabLocation.scEstimation ?
                    <ScoreView univId={this.state.currentUnivId} />
                : curTab === TabLocation.formSub ?
                    <FillView univId={this.state.currentUnivId} />
                : <div />}
            </div>
        </div>
    }

    private _onTabChange(newTab: TabLocation): void {
        this.setState({
            currentTab: newTab,
        });
    }

    private _onUnivChange(univId: number | undefined): void {
        this.setState({
            currentUnivId: univId,
        });
    }
}
