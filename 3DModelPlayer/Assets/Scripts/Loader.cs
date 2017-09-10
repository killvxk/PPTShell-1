using UnityEngine;
using System.Collections;
using System;
using System.IO;

public class Loader : MonoBehaviour {
    private string m_strRsrcName = "";
    private GameObject m_objTarget;
    void Awake()
    {

        m_strRsrcName = GetRsrcName();
        if(m_strRsrcName == "")
        {
            enabled = false;
            return;
        }

//        targetposition.gameObject.SetActive(true);
    }

	// Use this for initialization
	void Start () {
        StartCoroutine(LoadAssetBundle());
	}

    IEnumerator LoadAssetBundle()
    {
        string strAssetName = "file:// " + m_strRsrcName;
        Debug.Log("LoadAssetBundle file name :" + strAssetName);
        WWW wwwAsset = new WWW(strAssetName);
        yield return wwwAsset;
        if (wwwAsset != null && string.IsNullOrEmpty(wwwAsset.error) && wwwAsset.assetBundle != null)
        {
            AssetBundle srcassets = wwwAsset.assetBundle;
            string[] arrAssetNames = srcassets.GetAllAssetNames();
            if (arrAssetNames.Length>0)
            {
                m_objTarget = GameObject.Instantiate(srcassets.LoadAsset(arrAssetNames[0])) as GameObject;
                m_objTarget.AddComponent<PlayControl>();
                Transform transTarget = m_objTarget.transform;

                //计算位置
                MeshFilter meshfltr = m_objTarget.GetComponent<MeshFilter>();
                if(meshfltr != null)
                {
                    Vector3 size = Vector3.zero;
                    size.x =  meshfltr.mesh.bounds.size.x * m_objTarget.transform.localScale.x;
                    size.y = meshfltr.mesh.bounds.size.y * m_objTarget.transform.localScale.y;
                    size.z = meshfltr.mesh.bounds.size.z * m_objTarget.transform.localScale.z;
                    float fWidth = size.x>size.y ? size.x : size.y;
                    fWidth = fWidth > size.z ? fWidth : size.z;
                    float fDis = fWidth / Mathf.Tan(Camera.main.fieldOfView / 2 * Mathf.Deg2Rad);
                    Vector3 vt3Dest = Camera.main.transform.position + Camera.main.transform.forward * fDis;
//                    transTarget.position = vt3Dest;
                    transTarget.position = vt3Dest + new Vector3(0, -1*fWidth*0.4f,0);
                }

                //shader更新成新版的
                Renderer[] arrRender = m_objTarget.GetComponentsInChildren<Renderer>(true);
                for(int i=0; i<arrRender.Length; i++)
                {
                    Shader sdr = arrRender[i].material.shader;
                    string strShader = sdr.ToString();
                    strShader = strShader.Substring(0, strShader.IndexOf("(")-1);
                    Shader sdrNew = Shader.Find(strShader);
                    if (sdrNew != null)
                        arrRender[i].material.shader = sdrNew;
                }
            }
            srcassets.Unload(false);
            
        }
    }

    string GetRsrcName()
    {
        string strCommandLine = System.Environment.CommandLine;
//        strCommandLine = "3DPlayer.exe -dE:\\unity3D\\资源\\植物\\【花11】【一株、紫色、尖白】【翁刘斌】\\Assetsbundle\\windows\\花11  ";

        int iStartPos = strCommandLine.IndexOf("-d");
        string strPath = "";
        strPath = strCommandLine.Substring(iStartPos + 2);
        strPath = strPath.Trim();

        if (!File.Exists(strPath))
        {
            strPath = System.Environment.CurrentDirectory + "/Sample/花11";
        }
        strPath = strPath.Replace('\\', '/');

        return strPath;

//         string strCfgFileName = System.Environment.CurrentDirectory + "/Path.txt";
//         strCfgFileName = strCfgFileName.Replace('\\', '/');
// 
//         if(!File.Exists(strCfgFileName))
//         {
//             Debug.LogError(string.Format("Path config file not exist!! file:{0}", strCfgFileName));
//             return "";
//         }
// 
//         TextReader txtreader = new StreamReader(strCfgFileName);
//         string strContent = txtreader.ReadToEnd();
//         txtreader.Close();
//         strContent = strContent.Trim();
//         strContent = strContent.Replace('\\', '/');
//         return strContent;
    }


}
