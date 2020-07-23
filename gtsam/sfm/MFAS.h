/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010-2020, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

#pragma once

#include <gtsam/geometry/Unit3.h>
#include <gtsam/inference/Key.h>

#include <map>
#include <memory>
#include <vector>

namespace gtsam {

/**
  The MFAS class to solve a Minimum feedback arc set (MFAS)
  problem. We implement the solution from:
  Kyle Wilson and Noah Snavely, "Robust Global Translations with 1DSfM", 
  Proceedings of the European Conference on Computer Vision, ECCV 2014

  Given a weighted directed graph, the objective in a Minimum feedback arc set
  problem is to obtain a directed acyclic graph by removing
  edges such that the total weight of removed edges is minimum.
  @addtogroup SFM
*/
class MFAS {
 public:
  // used to represent edges between two nodes in the graph
  using KeyPair = std::pair<Key, Key>;
  using TranslationEdges = std::map<KeyPair, Unit3>;
 private:
  // pointer to nodes in the graph
  const std::shared_ptr<std::vector<Key>> nodes_;

  // edges with a direction such that all weights are positive
  // i.e, edges that originally had negative weights are flipped
  std::map<KeyPair, double> edgeWeights_;

 public:
  /**
   * @brief Construct from the nodes in a graph and weighted directed edges
   * between the graph. A shared pointer to the nodes is used as input parameter. 
   * This is because, MFAS ordering is usually used to compute the ordering of a 
   * large graph that is already stored in memory. It is unnecessary to copy the 
   * set of nodes in this class. 
   * @param nodes: Nodes in the graph
   * @param edgeWeights: weights of edges in the graph (map from pair of keys 
   * to signed double)
   */
  MFAS(const std::shared_ptr<std::vector<Key>> &nodes,
       const std::map<KeyPair, double> &edgeWeights) :
       nodes_(nodes), edgeWeights_(edgeWeights) {}

  /**
   * @brief Constructor for using in the context of translation averaging. Here, 
   * the nodes of the graph are cameras in 3D and the edges have a unit translation 
   * direction between them. The weights of the edges is computed by projecting 
   * them along a projection direction. 
   * @param nodes Nodes in the graph
   * @param relativeTranslations translation directions between nodes
   * @param projectionDirection direction in which edges are to be projected
   */
  MFAS(const std::shared_ptr<std::vector<Key>> &nodes,
       const TranslationEdges& relativeTranslations,
       const Unit3 &projectionDirection);

  /**
   * @brief Computes the "outlier weights" of the graph. We define the outlier weight
   * of a edge to be zero if the edge is an inlier and the magnitude of its edgeWeight
   * if it is an outlier.
   * @return outlierWeights: map from an edge to its outlier weight.
   */
  std::map<KeyPair, double> computeOutlierWeights() const;

  /**
   * @brief Computes the 1D MFAS ordering of nodes in the graph
   * @return orderedNodes: vector of nodes in the obtained order
   */
  std::vector<Key> computeOrdering() const;
};

}  // namespace gtsam
