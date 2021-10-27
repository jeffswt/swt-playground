
import { FacilityEntry } from "../Models/DbModels";

export { extractScore };

function extractScore(entry: FacilityEntry): number {
    let years: {[key: number]: number} = {};
    for (let i of entry.baselines)
        if (i.province === '北京') {
            if (i.year in years)
                years[i.year] = Math.max(years[i.year], i.score);
            else
                years[i.year] = i.score;
        }
    let mxyr = -1;
    let res = 0;
    for (let i of Object.keys(years))
        if (parseInt(i) >= mxyr) {
            mxyr = parseInt(i);
            res = years[parseInt(i)];
        }
    return res;
}
