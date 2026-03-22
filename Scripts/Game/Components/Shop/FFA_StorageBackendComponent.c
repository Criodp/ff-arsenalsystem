[ComponentEditorProps(category: "GameLib/Scripted", description: "Storage Backend for Arsenal")]
class FFA_StorageBackendComponentClass : JWK_ShopBackendComponentClass
{
};

modded enum JWK_EShopBackendType
{
	ARSENAL
}

class FFA_StorageBackendComponent : JWK_ShopBackendComponent
{
	protected ref map<ResourceName, int> m_mStorageMap = new map<ResourceName, int>();

	void GetStoredData(out array<ResourceName> outItems, out array<int> outCounts)
	{
		for (int i = 0; i < m_mStorageMap.Count(); i++)
		{
			outItems.Insert(m_mStorageMap.GetKey(i));
			outCounts.Insert(m_mStorageMap.GetElement(i));
		}
	}

	void SetStoredData(array<ResourceName> items, array<int> counts)
	{
		m_mStorageMap.Clear();
		if (!items || !counts) return;

		for (int i = 0; i < items.Count(); i++)
		{
			m_mStorageMap.Insert(items[i], counts[i]);
		}
	}
}