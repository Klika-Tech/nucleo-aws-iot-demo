import React from 'react';
import { SidebarNav, SidebarNavItem } from '@sketchpixy/rubix';

const Menu = () => (
    <div className="sidebar-nav-container">
        <SidebarNav>
            <SidebarNavItem
                name="Dashboard" href="/dashboard"
                glyph="icon-fontello-th-large"
            />
            <SidebarNavItem
                name="Temperature" href="/temperature"
                glyph="icon-fontello-temperatire"
            />
            <SidebarNavItem
                name="Humidity" href="/humidity"
                glyph="icon-fontello-water"
            />
            <SidebarNavItem
                name="Barometer" href="/barometer"
                glyph="icon-fontello-gauge"
            />
            <SidebarNavItem
                name="Magnetometer" href="/magnetometer"
                glyph="icon-fontello-magnet"
            />
            <SidebarNavItem
                name="Gyroscope" href="/gyroscope"
                glyph="icon-fontello-direction"
            />
            <SidebarNavItem
                name="Accelerometer" href="/accelerometer"
                glyph="icon-fontello-chart-line"
            />
        </SidebarNav>
    </div>
);

export default Menu;
