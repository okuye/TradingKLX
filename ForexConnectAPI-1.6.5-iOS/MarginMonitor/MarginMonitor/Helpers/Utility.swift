import Foundation
import UIKit

extension Float {
    func parseToPlaces(places:Int) -> String {
        let formatStr = String(format: "%%.%df", places);
        return String(format: formatStr, self)
    }
}

extension Double {
    func parseToPlaces(places:Int) -> String {
        let formatStr = String(format: "%%.%df", places);
        return String(format: formatStr, self)
    }
}

class Utility {
    static func amountToString(instrument: String, amount: Int) -> String {
        let isForexInstrument = ForexConnect.sharedInstance().isForexInstrument(instrument: instrument)
        var result: String
        if isForexInstrument {
            result = "\(amount/1000)K"
        } else {
            result = "\(amount)"
        }
        return result
    }
}
