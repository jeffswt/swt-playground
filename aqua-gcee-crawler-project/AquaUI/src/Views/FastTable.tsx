
import React from 'react';
import '../Styles/FastTable.scss';

export { FastTable };

interface IProps {
    data: {[key: string]: any}[];
    titles: {[key: string]: string};
    cols: string[];
}

interface IState {}

class FastTable extends React.Component<IProps, IState> {
    props: IProps;

    constructor(props: IProps) {
        super(props);
        this.props = props;
        this.state = {};
    }

    render() {
        return <div className="fast-table">
            <table>
                <thead>
                    {this.props.cols.map((col) => <td>
                        {this.props.titles[col]}</td>)}
                </thead>
                <tbody>
                    {this.props.data.map((row) => this._row(row))}
                </tbody>
            </table>
        </div>
    }

    private _row(row: {[key: string]: any}) {
        let tds = [];
        for (let col of this.props.cols) {
            let unit = '';
            if (col in row)
                unit = row[col].toString();
            tds.push(<td>{unit}</td>);
        }
        return <tr>{tds}</tr>
    }
}
