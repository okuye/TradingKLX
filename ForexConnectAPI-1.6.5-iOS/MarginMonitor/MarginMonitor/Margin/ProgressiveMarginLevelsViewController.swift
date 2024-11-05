import Foundation
import UIKit

class ProgressiveMarginLevelsViewController : UITableViewController {
    private var tradesTable: TradesTable?
    public var currentInstrument: String?
    private var totalTradesAmount: String?
    private var progressiveMarginLevelsModel: [ProgressiveMarginLevelModel]?
    
    override func viewDidLoad() {
        super.viewDidLoad()

        self.tableView.register(UITableViewCell.self, forCellReuseIdentifier: "cell")
        self.tableView.dataSource = self;
        self.tableView.delegate = self;
        self.title = "Progressive Margin";
    }
    
    override func viewWillAppear(_ animated: Bool) {
        tradesTable = TradesTable()
        tradesTable?.subscribeTradesUpdates(closure: tradesTableChanged)
        tradesTable?.load()
        
        progressiveMarginLevelsModel = ProgressiveMarginProvider.progressiveMargin(instrument: currentInstrument ?? "")
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        tradesTable!.unsubscribeTradesUpdates()
        tradesTable = nil
    }
    
    override func numberOfSections(in tableView: UITableView) -> Int {
        return 2
    }
    
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if section == 0 {
            return 1
        } else {
            return progressiveMarginLevelsModel?.count ?? 0
        }
    }
    
    override func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        if section == 0 {
            return "Symbol            Current Position Size"
        } else {
            return "Tier            Amount      Margin per contract"
        }
    }
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        let isFirstSection = (indexPath.section == 0)
        let cell = isFirstSection ? dequeueCellForFirstSection() : dequeueCellForSecondSection()
        
        if progressiveMarginLevelsModel == nil { return cell }
        
        // the first section: "Symbol" and "Current Position Size"
        
        if isFirstSection {
            let instrumentLabel = cell.textLabel!
            let currPosSizelabel = cell.contentView.viewWithTag(2) as! UILabel
            instrumentLabel.text = currentInstrument ?? ""
            currPosSizelabel.text = totalTradesAmount ?? ""
            return cell
        }
        
        // the second section: "Tier", "Amount" and "Margin per contract"
        
        let row = indexPath.row
        
        let tierLabel = cell.textLabel!
        let amountLabel = cell.contentView.viewWithTag(2) as! UILabel
        let marginLabel = cell.contentView.viewWithTag(3) as! UILabel
        
        tierLabel.text = progressiveMarginLevelsModel![row].tier
        amountLabel.text = progressiveMarginLevelsModel![row].amount
        marginLabel.text = progressiveMarginLevelsModel![row].marginPerContract
  
        return cell
    }
    
    func dequeueCellForFirstSection() -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "cell")! as UITableViewCell
        
        if cell.contentView.viewWithTag(2) == nil {
            
            let secondLabel = UILabel(frame: CGRect(x: 150, y: 10.0, width: 80.0, height: 25.0))
            secondLabel.tag = 2
            secondLabel.textAlignment = NSTextAlignment.right
            secondLabel.font = UIFont.systemFont(ofSize: 16.0)
            secondLabel.textColor = UIColor.black
            secondLabel.autoresizingMask = UIViewAutoresizing.flexibleHeight
            cell.contentView.addSubview(secondLabel)
        }
        return cell
    }
    
    func dequeueCellForSecondSection() -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "cell")! as UITableViewCell
        
        if cell.contentView.viewWithTag(2) == nil {
            
            let secondLabel = UILabel(frame: CGRect(x: 20.0, y: 10.0, width: 130.0, height: 25.0))
            secondLabel.tag = 2
            secondLabel.textAlignment = NSTextAlignment.right
            secondLabel.font = UIFont.systemFont(ofSize: 16.0)
            secondLabel.textColor = UIColor.black
            secondLabel.autoresizingMask = UIViewAutoresizing.flexibleHeight
            cell.contentView.addSubview(secondLabel)
        
            let thirsLabel = UILabel(frame: CGRect(x: 190.0, y: 10.0, width: 80.0, height: 25.0))
            thirsLabel.tag = 3
            thirsLabel.textAlignment = NSTextAlignment.right
            thirsLabel.font = UIFont.systemFont(ofSize: 16.0)
            thirsLabel.textColor = UIColor.black
            thirsLabel.autoresizingMask = UIViewAutoresizing.flexibleHeight
            cell.contentView.addSubview(thirsLabel)
        }
        return cell
    }
    
    func tradesTableChanged(tradesTableRows: [TradesTableRow]) {
        let totalAmount = tradesTable?.calcTotalAmount(instrument: currentInstrument ?? "") ?? 0
        totalTradesAmount = Utility.amountToString(instrument: currentInstrument ?? "", amount: totalAmount)

        DispatchQueue.main.async {
            self.tableView.reloadData()
        }
    }
}
