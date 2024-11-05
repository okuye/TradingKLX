import UIKit

class OffersViewController : UITableViewController {
    
    var headerLabel: UILabel?
    
    var currentHeaderLabel: UIView?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        title = "Rates";
        
        tableView.dataSource = self;
        tableView.delegate = self;
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        let forexConnect = ForexConnect.sharedInstance()
        forexConnect.subscribeOffersUpdates(closure: offersUpdated)
        headerLabel = createHeaderLabel()
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        
        let forexConnect = ForexConnect.sharedInstance()
        forexConnect.unsubscribeOffersUpdates()
    }
    
    override func numberOfSections(in tableView: UITableView) -> Int {
        return 1;
    }
    
    override func tableView(_ tableView: UITableView, viewForHeaderInSection section: Int) -> UIView? {
        return headerLabel
    }
    
    override func tableView(_ tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        return 40.0
    }
    
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        let forexConnect = ForexConnect.sharedInstance()
        return forexConnect.offersCount()
    }
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {

        let forexConnect = ForexConnect.sharedInstance()
        let cell = dequeueCell()
        
        let instr = forexConnect.getInstrument(index: indexPath.row)
        let bid = forexConnect.getBid(index: indexPath.row)
        let ask = forexConnect.getAsk(index: indexPath.row)
        let bidDirection = forexConnect.getBidDirection(index: indexPath.row)
        let askDirection = forexConnect.getAskDirection(index: indexPath.row)
        let digits = forexConnect.getDigits(index: indexPath.row)
        let mmr = forexConnect.calcMMR(instrument: instr)
       
        let instrumentLabel = cell.textLabel!
        let bidLabel = cell.contentView.viewWithTag(2) as! UILabel
        let askLabel = cell.contentView.viewWithTag(3) as! UILabel
        let mmrLabel = cell.contentView.viewWithTag(4) as! UILabel
        
        instrumentLabel.text = instr
        bidLabel.text = bid.parseToPlaces(places: digits)
        askLabel.text = ask.parseToPlaces(places: digits)
        mmrLabel.text = mmr.parseToPlaces(places: 2)
        
        if  bidDirection > 0 {
            bidLabel.textColor = UIColor.red
        }
        else if bidDirection < 0 {
            bidLabel.textColor = UIColor.blue
        }
        else {
            bidLabel.textColor = UIColor.black
        }
        
        if  askDirection > 0 {
            askLabel.textColor = UIColor.red
        }
        else if askDirection < 0 {
            askLabel.textColor = UIColor.blue
        }
        else {
            askLabel.textColor = UIColor.black
        }
        
        return cell
    }
    
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        let forexConnect = ForexConnect.sharedInstance()
        
        let myIndexPath = self.tableView.indexPathForSelectedRow!.row

        let instr = forexConnect.getInstrument(index: myIndexPath)
        let isProgressiveMarginSupported = forexConnect.isProgressiveMarginSupported(instrument: instr)
        if !isProgressiveMarginSupported {
            showErrorAlert(msg: "The progressive margin is not applicable to \(instr)")
            return
        }
        
        let mainStoryboard = UIStoryboard(name: "Main", bundle: Bundle.main)
        let progressiveMarginLevelsVC = mainStoryboard.instantiateViewController(withIdentifier: "ProgressiveMarginLevelsViewController") as! ProgressiveMarginLevelsViewController
        progressiveMarginLevelsVC.currentInstrument = forexConnect.getInstrument(index: myIndexPath)
        self.navigationController?.pushViewController(progressiveMarginLevelsVC, animated: true)
    }
    
    func offersUpdated() {
        DispatchQueue.main.async {
            self.tableView.reloadData()
        }
    }
    
    func createHeaderLabel() -> UILabel {
        let headerLabel = UILabel(frame: CGRect(x: 30.0, y: 10.0, width: 600.0, height: 25.0))
        headerLabel.textAlignment = NSTextAlignment.left
        headerLabel.textColor = UIColor.black
        headerLabel.backgroundColor = UIColor.gray
        headerLabel.autoresizingMask = UIViewAutoresizing.flexibleHeight
        headerLabel.text = "    Symbol              Bid           Ask             MMR  "
   
        return headerLabel
    }
    
    func dequeueCell() -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "cell")! as UITableViewCell
        
        if cell.contentView.viewWithTag(2) == nil {
            
            let bidLabel = UILabel(frame: CGRect(x: 100.0, y: 10.0, width: 80.0, height: 25.0))
            bidLabel.tag = 2
            bidLabel.textAlignment = NSTextAlignment.right
            bidLabel.font = UIFont.systemFont(ofSize: 16.0)
            bidLabel.textColor = UIColor.black
            bidLabel.autoresizingMask = UIViewAutoresizing.flexibleHeight
            cell.contentView.addSubview(bidLabel)
        
            let askLabel = UILabel(frame: CGRect(x: 180.0, y: 10.0, width: 80.0, height: 25.0))
            askLabel.tag = 3
            askLabel.textAlignment = NSTextAlignment.right
            askLabel.font = UIFont.systemFont(ofSize: 16.0)
            askLabel.textColor = UIColor.black
            askLabel.autoresizingMask = UIViewAutoresizing.flexibleHeight
            cell.contentView.addSubview(askLabel)
            
            let mmrLabel = UILabel(frame: CGRect(x: 240.0, y: 10.0, width: 100.0, height: 25.0))
            mmrLabel.tag = 4
            mmrLabel.textAlignment = NSTextAlignment.right
            mmrLabel.font = UIFont.systemFont(ofSize: 16.0)
            mmrLabel.textColor = UIColor.black
            mmrLabel.autoresizingMask = UIViewAutoresizing.flexibleHeight
            cell.contentView.addSubview(mmrLabel)
        }
        return cell
    }
    
    func showErrorAlert(msg: String)  {
        let alert = UIAlertView()
        alert.message = msg
        alert.addButton(withTitle: "ok")
        alert.show()
    }
}
