/* ========================================================================== */
/*   Main.cs                                                                  */
/*   Connection between the game and the Unity engine.                        */
/* -------------------------------------------------------------------------- */
/*   Copyright (c) 2011 Laurens Rodriguez Oscanoa.                            */
/*   This code is licensed under the MIT license:                             */
/*   http://www.opensource.org/licenses/mit-license.php                       */
/* -------------------------------------------------------------------------- */

using UnityEngine;
using System;
using Stc;

public class Bridge : MonoBehaviour
{
    public static GameObject Instantiate(string resourceName, Vector3 position)
    {
        try
        {
            return (GameObject)Instantiate(Resources.Load(resourceName), position, Quaternion.identity);
        }
        catch (Exception error)
        {
            Debug.LogError("Instantiate[" + resourceName + "] " + error);
            return null;
        }
    }

    public static void Destroy(GameObject clip)
    {
        Destroy(clip, 0);
    }

    public void Start()
    {
        m_game = new Game();
        m_game.Init(new Platform());
    }

    void Update()
    {
        m_game.Update();
    }

    private Game m_game = null;
}
