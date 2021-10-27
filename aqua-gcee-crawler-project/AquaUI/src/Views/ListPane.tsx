
import React from 'react';
import * as FaIcons from '@fortawesome/free-solid-svg-icons'
import { DbLoader } from '../Data/DbLoader';
import { FacilityEntry } from '../Models/DbModels';
import { ListPaneButton } from './ListPaneButton';
import '../Styles/ListPane.scss';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';

export { ListPane };

interface IProps {
    onClick?: (univId: number | undefined) => void;
}

interface IState {
    _ticker: number;
    clicked?: number;  // currently clicked univ_id
}

class ListPane extends React.Component<IProps, IState> {
    props: IProps;
    univList: FacilityEntry[];
    filterBuffer: string;

    constructor(props: IProps) {
        super(props);
        this.props = props;
        this.state = {
            _ticker: 0,
            clicked: undefined,
        };
        this.univList = [];
        this.filterBuffer = '';
    }

    componentDidMount () {
        this._load();
    }

    async _load(): Promise<void> {
        if (this.univList.length > 0)
            return;
        this.univList = await DbLoader.getList();
        this.setState({
            _ticker: this.state._ticker + 1,
        });
    }

    render() {
        let grammar = (
            '所有关键字需用空格隔开，支持模糊搜索和特殊查询：\n' +
            '　*　985 = 必须是985院校；\n' +
            '　*　211 = 必须是211院校；\n' +
            '　*　双一流/11 = 必须是双一流院校；\n' +
            '　*　!985 = 不得是985院校；\n' +
            '　*　!211 = 不得是211院校；\n' +
            '　*　!双一流 / !11 = 不得是双一流院校；\n' +
            '此外还支持使用 >=... 和 <=... 语句筛选投档线：\n' +
            '　*　但是符号与数字之间不得存在空格，\n' +
            '　*　例如 >=678 <=123。'
        );
        return <div className="list-pane">
            <div className="filter">
                <div className="input"
                    title={grammar}>
                    <input type="text"
                        placeholder="空格分隔 (与)关键字筛选院校"
                        onChange={(x) => this.filterBuffer = x.target.value} />
                    <div className="do-filter" onClick={
                        () => this._filter(this.filterBuffer)}>
                        <FontAwesomeIcon icon={FaIcons.faSearch} />
                    </div>
                </div>
            </div>
            <div className="items">
                {this.univList.map((entry) =>
                    <ListPaneButton key={entry.src_id} entry={entry}
                            active={this.state.clicked === entry.src_id}
                            icon={FaIcons.faChevronRight}
                            onClick={() => this._onClick(entry.src_id)}
                    />)}
            </div>
        </div>
    }

    private _onClick(univId: number): void {
        if (univId === this.state.clicked) {
            this.setState({
                clicked: undefined,
            });
            if (this.props.onClick !== undefined)
                this.props.onClick(undefined);
            return;
        }
        this.setState({
            clicked: univId,
        });
        if (this.props.onClick !== undefined)
            this.props.onClick(univId);
    }

    private async _filter(filter: string): Promise<void> {
        // first reset the list
        let list = await DbLoader.getList();
        // split the expressions
        let exprs = filter.split(/ +/).map((x) => x.trim());
        let res: FacilityEntry[] = [];
        // match
        for (let i of list) {
            let accept = true;
            for (let expr of exprs)
                if (!this._lookup(i, expr)) {
                    accept = false;
                    break;
                }
            if (accept)
                res.push(i);
        }
        // update
        this.univList = res;
        this.setState({
            _ticker: this.state._ticker + 1,
        });
    }

    private _lookup(i: FacilityEntry, expr: string): boolean {
        if (expr.length === 0)
            return true;
        // aliases
        if (expr === '工信部')
            expr = '工业和信息化部';
        // special
        if (expr === '985')
            return i.props.is_985;
        if (expr === '!985')
            return !i.props.is_985;
        if (expr === '211')
            return i.props.is_211;
        if (expr === '!211')
            return !i.props.is_211;
        if (expr === '双一流' || expr === '11')
            return i.props.is_fc.length > 0;
        if (expr === '!双一流' || expr === '!11')
            return i.props.is_fc === '';
        // score filter
        if (expr.startsWith('>='))
            return i.evalScore >= parseInt(expr.substr(2));
        if (expr.startsWith('<='))
            return i.evalScore <= parseInt(expr.substr(2));
        // vague match
        if (i.fac_name && i.fac_name.includes(expr))
            return true;
        if (i.contact.location_pv && i.contact.location_pv.includes(expr))
            return true;
        if (i.contact.location_dc && i.contact.location_dc.includes(expr))
            return true;
        if (i.fac_type && i.fac_type.includes(expr))
            return true;
        if (i.fac_pub && i.fac_pub.includes(expr))
            return true;
        if (i.fac_group && i.fac_group.includes(expr))
            return true;
        if (i.props.affiliate && i.props.affiliate.includes(expr))
            return true;
        // nope
        return false;
    }
}
