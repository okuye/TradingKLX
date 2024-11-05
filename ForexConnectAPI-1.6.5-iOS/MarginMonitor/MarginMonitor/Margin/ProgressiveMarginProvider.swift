import Foundation

class ProgressiveMarginLevelModel
{
    public let tier: String
    public let amount: String
    public let marginPerContract: String
    
    init(tier: Int, from: Int, till: Int, multiplier: Double,  isForexInstrument: Bool) {
        self.tier = "\(tier)"
        
        if till == -1 {
            self.amount =  isForexInstrument ? "\(from)K+" : "\(from)+"
        } else {
            self.amount =  isForexInstrument ? "\(from)K-\(till)K" : "\(from)-\(till)"
        }
        self.marginPerContract = multiplier.parseToPlaces(places: 2)
    }
}

class ProgressiveMarginProvider {
    
    static func progressiveMargin(instrument: String) -> [ProgressiveMarginLevelModel]? {
        let forexConnect = ForexConnect.sharedInstance()
        let isSupported = forexConnect.isProgressiveMarginSupported(instrument: instrument)
        if isSupported == false { return nil }
        
        return forexConnect.calcProgressiveMarginLevels(instrument: instrument)
    }
}
