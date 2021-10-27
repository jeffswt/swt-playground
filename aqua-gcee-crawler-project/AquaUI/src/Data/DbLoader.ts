
import { FacilityEntry } from "../Models/DbModels";
import { extractScore } from "./ScoreEstimator";

export { DbLoader };

class DbLoader {
    // singleton model
    private static _instance = new DbLoader();
    static async getList(): Promise<FacilityEntry[]> {
        return await DbLoader._instance._getList();
    }

    static getListSync(): FacilityEntry[] {
        return DbLoader._instance._entries;
    }

    // instance
    private _loading: boolean;
    private _entries: FacilityEntry[];
    constructor() {
        this._loading = false;
        this._entries = [];
    }

    private async _load(): Promise<void> {
        if (this._loading)
            return;
        this._loading = true;
        this._entries = [];
        // prepare api options
        let options = {
            method: 'GET',
            headers: new Headers({
                'Content-Type': 'application/json',
                'UserName': 'test',
            }),
        };
        // fetch response
        let response = await fetch(`/megumin-data.json`, options);
        // parse result
        let data = await response.json();
        // add scores
        let entries = data as FacilityEntry[];
        for (let i of entries)
            i.evalScore = extractScore(i);
        entries.sort((a, b) => (b.evalScore - a.evalScore) * 100000 +
            (b.src_id - a.src_id));
        // update
        this._entries = entries;
        this._loading = false;
    }

    private async _getList(): Promise<FacilityEntry[]> {
        if (this._entries.length === 0)
            await this._load();
        return this._entries.map(x => x);
    }
}
