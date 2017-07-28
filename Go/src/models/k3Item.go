package models

type K3Item struct {
	FItemID    int     `json:"fitemid"`
	FNumber    string  `json:"fnumber"`
	FUnitName  string  `json:"funitname"`
	FName      string  `json:"fname"`
	FModel     string  `json:"fmodel"`
	FQty       float32 `json:"fqty"`
	FStockName string  `json:"fstockname"`
}

type K3ItemPoint struct {
	FItemID    *int
	FNumber    *string
	FUnitName  *string
	FName      *string
	FModel     *string
	FQty       *float32
	FStockName *string
}
