import React from 'react';
import { render } from 'react-dom';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import {
    Sidebar,
    SidebarBtn,
    Grid,
    Col,
    Row,
    MainContainer,
    PanelContainer,
    Panel,
    PanelBody,
    Navbar,
    NavbarHeader,
    NavbarBrand,
    Nav,
    NavItem,
    Icon,
    Tooltip,
    OverlayTrigger,
} from '@sketchpixy/rubix';
import classNames from 'classnames';
import { AWS_CONNECT } from '../../../actionTypes';
import Loader from '../../common/Loader';
import Menu from './Menu';
import './style.scss';

const MainContainerWR = withRouter(MainContainer);

const mapStateToProps = state => ({
    loader: !state.data.fetched,
    online: state.mqtt.connected,
});

class Main extends React.Component {

    componentDidMount() {
        const { dispatch } = this.props;
        dispatch({ type: AWS_CONNECT });
    }

    render() {
        const { loader, online } = this.props;
        if (loader) {
            return (
                <div className="app">
                    <div className="loader-container">
                        <Loader />
                    </div>
                </div>
            );
        }

        const onlineTooltip = (
            <Tooltip id="online-tooltip">
                The board is {online ? 'online' : 'offline'}
            </Tooltip>
        );
        const statusClassName = classNames('online-status', { online });

        const wrapInPanel = (children) => {
            if (this.props.location.pathname === '/dashboard') {
                return children;
            }
            return (
                <Grid>
                    <Row>
                        <Col xs={12}>
                            <PanelContainer className="full-screen">
                                <Panel>
                                    <PanelBody>{children}</PanelBody>
                                </Panel>
                            </PanelContainer>
                        </Col>
                    </Row>
                </Grid>
            );
        };

        return (
            <MainContainerWR>

                <div id="sidebar">
                    <div id="sidebar-container">
                        <Sidebar sidebar={0}>
                            <Grid>
                                <Row>
                                    <Col xs={12}>
                                        <Menu />
                                    </Col>
                                </Row>
                            </Grid>
                        </Sidebar>
                    </div>
                </div>

                <Grid id="navbar">
                    <Row>
                        <Col xs={12}>
                            <Navbar fixedTop fluid id="rubix-nav-header">
                                <Grid fluid>
                                    <Row>
                                        <Col xs={3} visible="xs">
                                            <SidebarBtn>
                                                <Nav className="pull-left visible-xs-inline-block">
                                                    <NavItem className="sidebar-btn" href="/">
                                                        <Icon bundle="fontello" glyph="th-list-5" />
                                                    </NavItem>
                                                </Nav>
                                            </SidebarBtn>
                                        </Col>
                                        <Col xs={6} sm={4}>
                                            <NavbarHeader>
                                                <NavbarBrand>Nucleo Board</NavbarBrand>
                                            </NavbarHeader>
                                        </Col>
                                        <Col xs={3} sm={8}>
                                            <Nav pullRight>
                                                <div className={statusClassName}>
                                                    <OverlayTrigger placement="left" overlay={onlineTooltip}>
                                                        <Icon bundle="fontello" glyph="circle" />
                                                    </OverlayTrigger>
                                                </div>
                                            </Nav>
                                        </Col>
                                    </Row>
                                </Grid>
                            </Navbar>
                        </Col>
                    </Row>
                </Grid>

                <div id="body">
                    {wrapInPanel(React.cloneElement(this.props.children))}
                </div>

            </MainContainerWR>
        );
    }

}

export default connect(mapStateToProps)(Main);
