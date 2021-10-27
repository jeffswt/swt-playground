
import React from 'react';
import '../Styles/SectionTitle.scss';

export { SectionTitle };

interface IProps {
    title: string;
}

interface IState {}

class SectionTitle extends React.Component<IProps, IState> {
    props: IProps;

    constructor(props: IProps) {
        super(props);
        this.props = props;
        this.state = {};
    }

    render() {
        return <div className="section-title">
            {this.props.title}
        </div>
    }
}
