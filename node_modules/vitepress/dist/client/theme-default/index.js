import './styles/fonts.css';
import './styles/vars.css';
import './styles/base.css';
import './styles/utils.css';
import './styles/components/custom-block.css';
import './styles/components/vp-code.css';
import './styles/components/vp-doc.css';
import './styles/components/vp-sponsor.css';
import Layout from './Layout.vue';
import NotFound from './NotFound.vue';
export { default as VPHomeHero } from './components/VPHomeHero.vue';
export { default as VPHomeFeatures } from './components/VPHomeFeatures.vue';
export { default as VPHomeSponsors } from './components/VPHomeSponsors.vue';
export { default as VPDocAsideSponsors } from './components/VPDocAsideSponsors.vue';
export { default as VPTeamPage } from './components/VPTeamPage.vue';
export { default as VPTeamPageTitle } from './components/VPTeamPageTitle.vue';
export { default as VPTeamPageSection } from './components/VPTeamPageSection.vue';
export { default as VPTeamMembers } from './components/VPTeamMembers.vue';
const theme = {
    Layout,
    NotFound
};
export default theme;
